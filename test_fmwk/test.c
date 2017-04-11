#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "logger.h"

/**
 * Convenience macro to save a couple lines of code
 */
#define FAIL_TEST do { \
        passed = 0; \
        goto out; \
    } while(0)

/**
 * Helper to print timestamped PASS or FAIL message
 *
 * @param result (i) result, 1 if passed, 0 if failed
 * @param test_name (i) test name to log
 * @return void
 */
static inline void 
print_result(int result, const char* test_name) {
    if (result) {
        logger(dbgInfo, "*** TestID: %s PASSED", test_name);
    } else {
        logger(dbgInfo, "*** TestID: %s FAILED", test_name);
    }
}


/** 
 * Test1: simple hello world
 */
void 
test1(const char *test_name) {
    int passed = 1;

    logger(dbgInfo, "Hello world, test '%s'", test_name);
    print_result(passed, test_name);
}

/** 
 * Test2: a bit more complicated to show how to fail tests
 */
void 
test2(const char *test_name) {
    int passed = 1;

    logger(dbgInfo, "test '%s'", test_name);
    if (1 != 0) {
        FAIL_TEST;
    }
out:
    print_result(passed, test_name);
}

test_arr_t Tests[] = 
{
    {"test1", test1},
    {"test2", test2}
};

int 
main(int argc, char *argv[])
{
    int i = 0;
    for (i = 0; i < sizeof(Tests) / sizeof(Tests[0]); i++) {
	logger(dbgInfo, "Running %s...", Tests[i].test_name);
	Tests[i].test_fn(Tests[i].test_name);
    }
    return 0;
}
