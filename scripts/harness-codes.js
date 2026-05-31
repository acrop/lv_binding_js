// Shared exit codes for scripts/run-tests.js and scripts/gui-test-runner.js.

/** A bundled GUI test finished its render window without crashing. */
export const TEST_EXIT_OK = 95;

/** The harness itself: all tests passed / some failed / internal error. */
export const HARNESS_EXIT_OK = 0;
export const HARNESS_EXIT_FAIL = 1;
export const HARNESS_EXIT_ERROR = 2;
