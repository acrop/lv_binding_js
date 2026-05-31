// Wrapper run by the test harness for each bundled GUI test:
//
//   lvgljs run scripts/gui-test-runner.js <test/index.js>
//
// Loads the test module (which starts the LVGL render loop) and exits with
// TEST_EXIT_OK once the render window elapses. TEST_RENDER_MS is inherited
// from the harness via the environment.

import path from 'tjs:path';

const scriptDir = import.meta.dirname;
const { TEST_EXIT_OK, HARNESS_EXIT_ERROR } = await import(path.join(scriptDir, 'harness-codes.js'));

const testArg = tjs.args[3];
const renderMs = parseInt(tjs.env.TEST_RENDER_MS || '1500', 10);

async function main() {
    if (!testArg) {
        console.error('usage: lvgljs run scripts/gui-test-runner.js <test.js>');
        tjs.exit(HARNESS_EXIT_ERROR);
        return;
    }

    const testPath = path.resolve(testArg);

    try {
        await import(testPath);
    } catch (e) {
        console.error('failed to load test:', e && e.stack ? e.stack : e);
        tjs.exit(HARNESS_EXIT_ERROR);
        return;
    }

    setTimeout(() => tjs.exit(TEST_EXIT_OK), renderMs);
}

main();
