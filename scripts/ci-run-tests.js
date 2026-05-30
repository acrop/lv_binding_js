// Cross-platform CI test runner for lvgljs.
//
// The bundled tests under test/**/index.js are GUI programs that start an
// event loop and never exit on their own. This runner launches each test with
// the lvgljs executable, lets it run for a short while, then terminates it.
//
// Pass/fail logic:
//   - exit code 0                      -> pass (clean exit)
//   - terminated by our timeout signal -> pass (rendered without crashing)
//   - any other non-zero exit / crash  -> fail
//
// Usage: node scripts/ci-run-tests.js <path-to-lvgljs-executable>
// Env:
//   TEST_TIMEOUT_MS  how long each test is allowed to run before being killed (default 5000)
//   TEST_GLOB        optional substring filter; only tests whose path contains it run

const { spawnSync } = require('child_process');
const fs = require('fs');
const path = require('path');

const exeArg = process.argv[2];
if (!exeArg) {
  console.error('Usage: node scripts/ci-run-tests.js <path-to-lvgljs-executable>');
  process.exit(2);
}
const exe = path.resolve(exeArg);
if (!fs.existsSync(exe)) {
  console.error(`Executable not found: ${exe}`);
  process.exit(2);
}

const repoRoot = path.resolve(__dirname, '..');
const testRoot = path.join(repoRoot, 'test');
const timeoutMs = parseInt(process.env.TEST_TIMEOUT_MS || '5000', 10);
const filter = process.env.TEST_GLOB || '';
const killSignal = 'SIGTERM';

function findTests(dir) {
  const found = [];
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const full = path.join(dir, entry.name);
    if (entry.isDirectory()) {
      found.push(...findTests(full));
    } else if (entry.isFile() && entry.name === 'index.js') {
      found.push(full);
    }
  }
  return found;
}

const tests = findTests(testRoot)
  .map((p) => path.relative(repoRoot, p).split(path.sep).join('/'))
  .filter((p) => p.includes(filter))
  .sort();

if (tests.length === 0) {
  console.error('No bundled tests found. Did you run "node build.js"?');
  process.exit(2);
}

console.log(`Found ${tests.length} test(s). Per-test timeout: ${timeoutMs}ms\n`);

const failures = [];

for (const test of tests) {
  const result = spawnSync(exe, ['run', test], {
    cwd: repoRoot,
    timeout: timeoutMs,
    killSignal,
    encoding: 'utf8',
  });

  const timedOut = result.signal === killSignal || (result.error && result.error.code === 'ETIMEDOUT');
  let ok;
  if (result.error && !timedOut) {
    ok = false;
  } else if (timedOut) {
    ok = true; // stayed alive for the whole window without crashing
  } else {
    ok = result.status === 0;
  }

  console.log(`${ok ? 'PASS' : 'FAIL'}  ${test}`);

  if (!ok) {
    failures.push(test);
    if (result.stdout) console.log('--- stdout ---\n' + result.stdout);
    if (result.stderr) console.log('--- stderr ---\n' + result.stderr);
    if (result.error) console.log('--- error ---\n' + result.error.message);
    console.log(`(exit status: ${result.status}, signal: ${result.signal})`);
  }
}

console.log(`\n${tests.length - failures.length}/${tests.length} test(s) passed.`);

if (failures.length > 0) {
  console.error('\nFailed tests:');
  for (const f of failures) console.error(`  - ${f}`);
  process.exit(1);
}
