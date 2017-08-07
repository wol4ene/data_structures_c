#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "trie_ext.h"
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

char *testArr[] = 
{
    "abc",
    "ben",
    "tom",
    "benny",
    "afar",
};
const int sizeArr = sizeof(testArr) / sizeof(testArr[0]);
/** 
 * Test1: empty trie gets created, verify count is 0
 */
void 
test1(const char *test_name) {
    int passed = 1;

    /* create empty trie */
    TriePtr t = trie_create(test_name);

    /* add words */
    int i = 0;
    for (i = 0; i < sizeArr; i++) {
        trie_insert(t, testArr[i]);
    }

    /* verify all expected words are present */
    for (i = 0; i < sizeArr; i++)
    if (0 == trie_search(t, testArr[i])) {
        logger(dbgErr, "Expected to find '%s', but didnt\n", testArr[i]);
        FAIL_TEST;
    };
    /* verify a few bogus words are not present */
    if (1 == trie_search(t, "foobar")) {  /* not present at all */
        logger(dbgErr, "Unexpectedly found 'foobar'\n");
        FAIL_TEST;
    }
    if (1 == trie_search(t, "benn")) {    /* present as a path, but not a leaf */
        logger(dbgErr, "Unexpectedly found 'benn'\n");
        FAIL_TEST;
    }

#if 0
    int got_cnt = trie_count(t);
    if (5 != got_cnt) {
        logger(dbgErr, "Expected %i nodes, instead has %i",
                5, got_cnt);
        FAIL_TEST;
    } else {
        logger(dbgInfo, "Got expected count of 0");
    };
#endif

    /* cleanup */
    trie_destroy(t);
    goto out;
out:
    print_result(passed, test_name);
}


test_arr_t Tests[] = 
{
    {"test1", test1},
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
