// lvgljs-native test harness, run *by lvgljs itself*:
//
//   lvgljs run scripts/run-tests.js [filter]
//
// Every bundled test under test/**/index.js is a GUI program that starts the
// LVGL event loop and never returns on its own. Each one is launched through
// scripts/gui-test-runner.js, which loads the test, lets it render for a
// short window, and then calls tjs.exit(TEST_EXIT_OK).
//
// Pass/fail logic:
//   - exit TEST_EXIT_OK, no signal, no error markers -> pass
//   - any other exit, crash/abort, killed, or error markers -> fail
//
// Env:
//   TEST_RENDER_MS   how long each GUI test renders before auto-exit (default 1500)
//   TEST_TIMEOUT_MS  hard kill safety net per test (default 15000)
//   TEST_GLOB        optional substring filter; only matching test paths run
//
// The harness is itself a lvgljs program, so it reports its verdict via
// tjs.exit() (HARNESS_EXIT_OK / HARNESS_EXIT_FAIL / HARNESS_EXIT_ERROR).

import path from 'tjs:path';

const scriptDir = import.meta.dirname;
const {
    TEST_EXIT_OK,
    HARNESS_EXIT_OK,
    HARNESS_EXIT_FAIL,
    HARNESS_EXIT_ERROR,
} = await import(path.join(scriptDir, 'harness-codes.js'));
const repoRoot = path.dirname(scriptDir);
const testRoot = path.join(repoRoot, 'test');
const runnerRel = 'scripts/gui-test-runner.js';

const filter = tjs.args[3] || tjs.env.TEST_GLOB || '';
const renderMs = parseInt(tjs.env.TEST_RENDER_MS || '1500', 10);
const hardTimeoutMs = parseInt(tjs.env.TEST_TIMEOUT_MS || '15000', 10);

const FAILURE_MARKERS = [
    /Assertion .* failed/,        // debug-build C assert (abort)
    /\(closed\) == \(1\)/,        // runtime teardown regression
    /AssertionError/,             // tjs:assert / node assert failures
    /\bUncaught\b/,               // uncaught exception dump
    /unhandled rejection/i,       // unhandled promise rejection
    /Segmentation fault/i,
];

async function listDir(dir) {
    const iter = await tjs.readDir(dir);
    const entries = [];
    for await (const entry of iter) {
        entries.push({ name: entry.name, isDir: entry.isDirectory, isFile: entry.isFile });
    }
    try {
        await iter.close();
    } catch (_) {
        // already closed when the iterator drained
    }
    return entries;
}

async function findTests(dir) {
    const found = [];
    for (const entry of await listDir(dir)) {
        if (entry.name === '.' || entry.name === '..') {
            continue;
        }
        const full = path.join(dir, entry.name);
        if (entry.isDir) {
            found.push(...await findTests(full));
        } else if (entry.isFile && entry.name === 'index.js') {
            found.push(full);
        }
    }
    return found;
}

function toRelative(p) {
    let rel = p.slice(repoRoot.length + 1);
    return rel.split('\\').join('/');
}

async function runTest(absPath, relPath) {
    tjs.env.TEST_RENDER_MS = String(renderMs);

    const proc = tjs.spawn([ tjs.exePath, 'run', runnerRel, relPath ], {
        cwd: repoRoot,
        stdout: 'pipe',
        stderr: 'pipe',
    });

    let killed = false;
    const killer = setTimeout(() => {
        killed = true;
        try {
            proc.kill('SIGKILL');
        } catch (_) {
            // already gone
        }
    }, hardTimeoutMs);

    let status, stdout, stderr;
    try {
        [ status, stdout, stderr ] = await Promise.all([
            proc.wait(),
            proc.stdout.text(),
            proc.stderr.text(),
        ]);
    } finally {
        clearTimeout(killer);
    }

    const combined = `${stdout || ''}\n${stderr || ''}`;
    const marker = FAILURE_MARKERS.find((re) => re.test(combined));

    let ok = true;
    let reason = '';
    if (killed) {
        ok = false;
        reason = `did not exit within ${hardTimeoutMs}ms (hard killed)`;
    } else if (status.term_signal) {
        ok = false;
        reason = `terminated by signal ${status.term_signal}`;
    } else if (status.exit_status !== TEST_EXIT_OK) {
        ok = false;
        reason = `exit status ${status.exit_status} (expected ${TEST_EXIT_OK})`;
    } else if (marker) {
        ok = false;
        reason = `output matched failure marker ${marker}`;
    }

    return { ok, reason, stdout, stderr };
}

async function main() {
    const tests = (await findTests(testRoot))
        .map((p) => ({ abs: p, rel: toRelative(p) }))
        .filter((t) => t.rel.includes(filter))
        .sort((a, b) => (a.rel < b.rel ? -1 : a.rel > b.rel ? 1 : 0));

    if (tests.length === 0) {
        console.log('No bundled tests found. Did you run "node build.js"?');
        tjs.exit(HARNESS_EXIT_ERROR);
        return;
    }

    console.log(`Found ${tests.length} test(s). Render window: ${renderMs}ms, hard timeout: ${hardTimeoutMs}ms\n`);

    const failures = [];
    for (const test of tests) {
        const result = await runTest(test.abs, test.rel);
        console.log(`${result.ok ? 'PASS' : 'FAIL'}  ${test.rel}`);
        if (!result.ok) {
            failures.push({ test, result });
            console.log(`      reason: ${result.reason}`);
            if (result.stdout) {
                console.log('--- stdout ---\n' + result.stdout);
            }
            if (result.stderr) {
                console.log('--- stderr ---\n' + result.stderr);
            }
        }
    }

    console.log(`\n${tests.length - failures.length}/${tests.length} test(s) passed.`);

    if (failures.length > 0) {
        console.log('\nFailed tests:');
        for (const f of failures) {
            console.log(`  - ${f.test.rel}`);
        }
        tjs.exit(HARNESS_EXIT_FAIL);
        return;
    }

    tjs.exit(HARNESS_EXIT_OK);
}

main().catch((e) => {
    console.log('harness error:', e && e.stack ? e.stack : e);
    tjs.exit(HARNESS_EXIT_ERROR);
});
