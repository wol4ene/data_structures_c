#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "slist_ext.h"
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
 * Helper to verify an slist is as expected
 *
 * If just testing for an empty list (exp_count == 0), the last
 * two args, exp_data and pos, will be ignored
 *
 * @param p (i) opaque pointer to slist
 * @param exp_count (i) expected num nodes in slist
 * @param exp_data  (i) expected data at node 'pos' in list
 * @param pos       (i) position in list to look at 'exp_data'
 * @return 1 if as expected, 0 if as not expected
 */
static int 
_slist_verify(ListPtr p, int exp_count, int exp_data, int pos)
{
    int passed = 1;
    int cur_count = slist_count(p);
    if (exp_count != cur_count) {
	logger(dbgCrit, "Expected list to have %i members, instead has %i\n", 
		exp_count, cur_count);
	FAIL_TEST;
    }
    if (exp_count > 0) {
	int cur_data = *(int*)slist_get_pos(p, pos);
	if (exp_data != cur_data) {
	    logger(dbgCrit, "Expected node at pos %i to have data = %i, instead has %i\n", 
		    pos, exp_data, cur_data);
	    FAIL_TEST;
	}
    }
out:
    return passed;
}

/** 
 * Test1: empty list gets created and gets of nodes return NULL
 */
void 
test1(const char *test_name) {
    int passed = 1;

    /* verify empty list gets created */
    ListPtr p = slist_new(test_name);
    if (0 != slist_count(p)) {
	logger(dbgCrit, "expected empty list, list has data\n");
	FAIL_TEST;
    }

    /* verify attempt to get non existant node returns NULL */
    if (NULL != slist_get_pos(p, 0)) {
        logger(dbgCrit, "expected to get NULL for get_pos, instead found data\n");
        FAIL_TEST;
    }
    if (NULL != slist_get_pos(p, 1)) {
        logger(dbgCrit, "expected to get NULL for get_pos, instead found data\n");
        FAIL_TEST;
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/** 
 * Test2: verify tail-adds functionality
 */
void 
test2(const char *test_name) {
    int passed = 1;
    int data0 = 5, data1 = 6;

    ListPtr p = slist_new(test_name);

    /* tail add node, verify count and data are expected */
    slist_add_tail(p, &data0);
    if (1 != _slist_verify(p, 1, data0, 0)) {
	FAIL_TEST;
    }

    /* tail add another node, verify count and data are expected */
    slist_add_tail(p, &data1);
    if (1 != _slist_verify(p, 2, data1, 1)) {
	FAIL_TEST;
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/** 
 * Test3: verify head-adds functionality
 */
void 
test3(const char *test_name) {
    int passed = 1;
    int data0 = 5, data1 = 6;

    ListPtr p = slist_new(test_name);

    /* head add node, verify count and data are expected */
    slist_add_head(p, &data0);
    if (1 != _slist_verify(p, 1, data0, 0)) {
	FAIL_TEST;
    }

    /* head add another node, verify count and data are expected */
    slist_add_head(p, &data1);
    if (1 != _slist_verify(p, 2, data1, 0)) {
	FAIL_TEST;
    }

    /* make sure data0 is now moved to pos 1*/
    if (1 != _slist_verify(p, 2, data0, 1)) {
	FAIL_TEST;
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/** 
 * Test4: verify permuations of add/del from head/tail
 */
void 
test4(const char *test_name) {
    int passed = 1;
    int data = 5;

    /* Test all 4 permutations of adding/del one item from head/tail */

    ListPtr p = slist_new(test_name);

    /* add one node to head, verify, delete head, verify empty */
    slist_add_head(p, &data);
    if (1 != _slist_verify(p, 1, data, 0)) {
	FAIL_TEST;
    }
    slist_del_head(p);
    if (1 != _slist_verify(p, 0, 0, 0)) {
	FAIL_TEST;
    }

    /* add one node to head, verify, delete tail, verify empty */
    slist_add_head(p, &data);
    if (1 != _slist_verify(p, 1, data, 0)) {
	FAIL_TEST;
    }
    slist_del_tail(p);
    if (1 != _slist_verify(p, 0, 0, 0)) {
	FAIL_TEST;
    }

    /* add one node to tail, verify, delete head, verify empty */
    slist_add_tail(p, &data);
    if (1 != _slist_verify(p, 1, data, 0)) {
	FAIL_TEST;
    }
    slist_del_head(p);
    if (1 != _slist_verify(p, 0, 0, 0)) {
	FAIL_TEST;
    }

    /* add one node to tail, verify, delete tail, verify empty */
    slist_add_tail(p, &data);
    if (1 != _slist_verify(p, 1, data, 0)) {
	FAIL_TEST;
    }
    slist_del_tail(p);
    if (1 != _slist_verify(p, 0, 0, 0)) {
	FAIL_TEST;
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/** 
 * Test5: verify long list of tail-adds in correct order
 */
void 
test5(const char *test_name) {
    int passed = 1;
    int arr[] = {1,2,3,4,5,6,7,8,9};
    int i = 0;
    int num_nodes = sizeof(arr)/sizeof(arr[0]);

    ListPtr p = slist_new(test_name);

    /* tail add 9 nodes */
    for (i = 0; i < num_nodes; i++) {
	slist_add_tail(p, &arr[i]);
    }

    /* verify each node is in its expected spot */
    for (i = 0; i < num_nodes; i++) {
	if (1 != _slist_verify(p, 9, arr[i], i)) {
	    FAIL_TEST;
	}
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/** 
 * Test6: verify long list of head-adds in correct order, then reverse and verify
 */
void 
test6(const char *test_name) {
    int passed = 1;
    int arr[] = {1,2,3,4,5,6,7,8,9};
    int i = 0;
    int num_nodes = sizeof(arr)/sizeof(arr[0]);

    ListPtr p = slist_new(test_name);

    /* head add 9 nodes */
    for (i = 0; i < num_nodes; i++) {
	slist_add_head(p, &arr[i]);
    }

    /* verify each node is in its expected spot
     * list will be 9,8,7,...,1 due to head add 
     */
    for (i = 0; i < num_nodes; i++) {
	if (1 != _slist_verify(p, 9, arr[num_nodes-1-i], i)) {
	    FAIL_TEST;
	}
    }

    /* reverse the list */
    slist_reverse(p);

    /* now verify list has been reversed */
    for (i = 0; i < num_nodes; i++) {
	if (1 != _slist_verify(p, 9, arr[i], i)) {
	    FAIL_TEST;
	}
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/**
 * Helper user by test below to add 2 to a given node in a callback
 */
static void
_slist_test_add_two(void *x)
{
   *(int*)x += 2;
}


/** 
 * Test7: verify the apply_fn gets called for each node in the list
 */
void 
test7(const char *test_name) {
    int passed = 1;
    int arr[] = {1,2,3,4,5,6,7,8,9};
    int orig[] = {1,2,3,4,5,6,7,8,9};
    int i = 0;
    int num_nodes = sizeof(arr)/sizeof(arr[0]);

    ListPtr p = slist_new(test_name);

    /* tail add 9 nodes */
    for (i = 0; i < num_nodes; i++) {
	slist_add_tail(p, &arr[i]);
    }

    /* apply a func to add 2 to each node, will modify arr */
    slist_apply_fn(p, _slist_test_add_two);

    /* now verify each node is +2 of the original */
    for (i = 0; i < num_nodes; i++) {
	if (1 != _slist_verify(p, 9, (orig[i]+2), i)) {
	    FAIL_TEST;
	}
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

/** 
 * Test8: verify operations don't core on an empty list
 */
void 
test8(const char *test_name) {
    int passed = 1;

    ListPtr empty_list = slist_new(test_name);

    /* run operations that might core on an empty list */
    slist_del_tail(empty_list);
    slist_del_head(empty_list);
    slist_reverse(empty_list);
    slist_apply_fn(empty_list, _slist_test_add_two);
    if (NULL != slist_get_pos(empty_list, 0)) {
        FAIL_TEST;
    }   
    if (0 != slist_count(empty_list)) {
        FAIL_TEST;
    }   

    /* cleanup */
    slist_destroy(empty_list);
out:
    print_result(passed, test_name);
}

test_arr_t Tests[] = 
{
    {"test1", test1},
    {"test2", test2},
    {"test3", test3},
    {"test4", test4},
    {"test5", test5},
    {"test6", test6},
    {"test7", test7},
    {"test8", test8}
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
