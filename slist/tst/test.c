#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "../inc/slist_ext.h"

void print_int(void *data) {
    printf("node: %i\n", *(int*)data);
}

#define FAIL_TEST do { \
    passed = 0; \
    goto out; \
    } while(0)


static inline void print_result(int result, const char* test_name) {
    if (result) {
	printf("*** TestID: %s PASSED\n", test_name);
    } else {
	printf("*** TestID: %s FAILED\n", test_name);
    }
}

static int _slist_verify(ListPtr p, int exp_count, int exp_data, int pos)
{
    int passed = 1;
    int cur_count = slist_count(p);
    if (exp_count != cur_count) {
	printf("Expected list to have %i members, instead has %i\n", 
		exp_count, cur_count);
	FAIL_TEST;
    }
    if (exp_count > 0) {
	int cur_data = *(int*)slist_get_pos(p, pos);
	if (exp_data != cur_data) {
	    printf("Expected node at pos %i to have data = %i, instead has %i\n", 
		    pos, exp_data, cur_data);
	    FAIL_TEST;
	}
    }
out:
    return passed;
}

void test1(const char *test_name) {
    int passed = 1;

    /* verify empty list gets created */
    ListPtr p = slist_new(test_name);
    if (0 != slist_count(p)) {
	printf("expected empty list, list has data\n");
	FAIL_TEST;
    }

    /* cleanup */
    slist_destroy(p);
out:
    print_result(passed, test_name);
}

void test2(const char *test_name) {
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

void test3(const char *test_name) {
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

void test4(const char *test_name) {
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

void test5(const char *test_name) {
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

#ifdef FALSE
    slist_apply_fn(p, print_int);
    
    int x = *(int*)slist_get_pos(p, 8);
    printf("Received x=%i\n", x);
    slist_destroy(p);
#endif 

test_arr_t Tests[] = 
{
    {"test1", test1},
    {"test2", test2},
    {"test3", test3},
    {"test4", test4},
    {"test5", test5}
};

int main(int argc, char *argv[])
{
    int i = 0;
    for (i = 0; i < sizeof(Tests) / sizeof(Tests[0]); i++) {
	printf("Running %s...\n", Tests[i].test_name);
	Tests[i].test_fn(Tests[i].test_name);
    }
    return 0;
}
