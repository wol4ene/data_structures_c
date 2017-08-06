#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "bintree_ext.h"
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
 * Test1: empty tree gets created, verify count is 0
 */
void 
test1(const char *test_name) {
    int passed = 1;

    /* create empty tree gets created */
    BintreePtr b = bintree_create(test_name);
   
    int got_cnt = bintree_count(b);
    if (0 != got_cnt) {
        logger(dbgErr, "Expected tree to have %i nodes, instead has %i",
                0, got_cnt);
        FAIL_TEST;
    } else {
        logger(dbgInfo, "Got expected count of 0");
    };

    /* cleanup */
    bintree_destroy(b);
    goto out;
out:
    print_result(passed, test_name);
}

/** 
 * Test2: create tree with one node, verify count is 1
 */
void 
test2(const char *test_name) {
    int passed = 1;

    int tree_nodes[] = {8};
    int num_nodes = sizeof(tree_nodes) / sizeof(tree_nodes[0]);
    int i = 0;

    /* create tree */
    BintreePtr b = bintree_create(test_name);
    
    /* add node to tree */
    for (i = 0; i < num_nodes; i++) {
        bintree_insert(b, tree_nodes[i]);
    }

    /* check count */
    int got_cnt = bintree_count(b);
    if (num_nodes != got_cnt) {
        logger(dbgErr, "Expected tree to have %i nodes, instead has %i",
                num_nodes, got_cnt);
        FAIL_TEST;
    } else {
        logger(dbgInfo, "Got expected count of %i nodes", num_nodes);
    }

    /* cleanup */
    bintree_destroy(b);
    goto out;
out:
    print_result(passed, test_name);
}

/** 
 * Test3: create tree with 7 nodes, verify count is 7
 *        do pre, post, inorder traversals
 */
void 
test3(const char *test_name) {
    int passed = 1;

    int tree_nodes[] = {8, 5, 11, 3, 9, 13, 7};
    int num_nodes = sizeof(tree_nodes) / sizeof(tree_nodes[0]);
    int i = 0;

    /* verify empty tree gets created */
    BintreePtr b = bintree_create(test_name);
    
    for (i = 0; i < num_nodes; i++) {
        bintree_insert(b, tree_nodes[i]);
    }

    bintree_preorder(b);
    bintree_inorder(b);
    bintree_postorder(b);

    int got_cnt = bintree_count(b);
    if (num_nodes != got_cnt) {
        logger(dbgErr, "Expected tree to have %i nodes, instead has %i",
                num_nodes, got_cnt);
        FAIL_TEST;
    } else {
        logger(dbgInfo, "Got expected count %i", num_nodes);
    }

    /* cleanup */
    bintree_destroy(b);
    goto out;
out:
    print_result(passed, test_name);
}

/** 
 * Test4: create tree with 7 nodes
 *        search for each node to make sure it is there
 *        search for a couple bogus nodes to ensure not there
 */
void 
test4(const char *test_name) {
    int passed = 1;

    int tree_nodes[] = {8, 5, 11, 3, 9, 13, 7};
    int bogus_nodes[] = {17, 1, 99};
    int num_nodes = sizeof(tree_nodes) / sizeof(tree_nodes[0]);
    int num_bogus_nodes = sizeof(bogus_nodes) / sizeof(bogus_nodes[0]);

    int i = 0;

    /* create tree */
    BintreePtr b = bintree_create(test_name);
    
    /* populate tree */
    for (i = 0; i < num_nodes; i++) {
        bintree_insert(b, tree_nodes[i]);
    }

    /* verify each node in tree exists */
    for (i = 0; i < num_nodes; i++) {
        if (0 == bintree_search(b, tree_nodes[i])) {
            logger(dbgErr, "Expected tree to contain %i, but it doesn't", tree_nodes[i]);
            FAIL_TEST;
        }
    }

    /* verify a couple bogus nodes do not exist */
    for (i = 0; i < num_bogus_nodes; i++) {
        if (1 == bintree_search(b, bogus_nodes[i])) {
            logger(dbgErr, "Tree unexpectely contains %i", bogus_nodes[i]);
            FAIL_TEST;
        }
    }

    /* cleanup */
    bintree_destroy(b);
    goto out;
out:
    print_result(passed, test_name);
}

/** 
 * Test5: create large tree and remove some nodes
 */
void 
test5(const char *test_name) {
    int passed = 1;

    int tree_nodes[] = {15, 14, 3, 4, 2, 1, 29, 20, 19, 17, 24, 22, 25, 26, 38, 36, 39};
    int num_nodes = sizeof(tree_nodes) / sizeof(tree_nodes[0]);
    int i = 0;

    /* verify empty tree gets created */
    BintreePtr b = bintree_create(test_name);
    
    /* populate tree */
    for (i = 0; i < num_nodes; i++) {
        bintree_insert(b, tree_nodes[i]);
    }

    bintree_inorder(b);

    /* remove 4 nodes */
    bintree_remove(b, 2);
    bintree_remove(b, 1);
    bintree_remove(b, 29);
    bintree_remove(b, 20);
    bintree_remove(b, 99);  /* expect this one not to remove anything */

    bintree_inorder(b);

    /* deleted 4 nodes above, verify the count is right */
    int got_cnt = bintree_count(b);
    if ((num_nodes-4) != got_cnt) {
        logger(dbgErr, "Expected tree to have %i nodes, instead has %i",
                (num_nodes-4), got_cnt);
        FAIL_TEST;
    }

    /* cleanup */
    bintree_destroy(b);
    goto out;
out:
    print_result(passed, test_name);
}

/** 
 * Test6: create large tree and find min/max, max depth, and hasPathSum
 */
void 
test6(const char *test_name) {
    int passed = 1;

    int tree_nodes[] = {15, 14, 3, 4, 2, 1, 29, 20, 19, 17, 24, 22, 25, 26, 38, 36, 39};
    int num_nodes = sizeof(tree_nodes) / sizeof(tree_nodes[0]);
    int i = 0;

    /* verify empty tree gets created */
    BintreePtr b = bintree_create(test_name);
    
    /* populate tree */
    for (i = 0; i < num_nodes; i++) {
        bintree_insert(b, tree_nodes[i]);
    }

    /* Test for min val == 1 and max_val == 39 */
    int got_min = bintree_minvalue(b);
    int got_max = bintree_maxvalue(b);
    logger(dbgInfo, "Found min=%i, max=%i", got_min, got_max);
    if (1 != got_min) {
        logger(dbgErr, "Expected min value 1, instead got: %i", got_min);
        FAIL_TEST;
    };
    if (39 != got_max) {
        logger(dbgErr, "Expected max value 39, instead got: %i", got_max);
        FAIL_TEST;
    };

    /* Test for maxdepth == 6 (path: 15, 29, 20, 24, 25, 26) */
    int got_depth = bintree_maxdepth(b);
    logger(dbgInfo, "Found max depth=%i", got_depth);
    if (6 != got_depth) {
        logger(dbgErr, "Expected max depth 6, instead got: %i", got_depth);
        FAIL_TEST;
    }

    /* Test for path sums 
     *   sum=36  (path: 15, 14, 3, 4)
     *   sum=110 (path: 15, 29, 20, 24, 22)
     *   sum=111 (path does not exist)
     */
    int hasSum = 0;
    hasSum = bintree_hasPathSum(b, 36);
    if (1 != hasSum) {
        logger(dbgErr, "Expected pathsum of 36, did not find it");
        FAIL_TEST;
    }
    hasSum = bintree_hasPathSum(b, 110);
    if (1 != hasSum) {
        logger(dbgErr, "Expected pathsum of 110, did not find it");
        FAIL_TEST;
    }
    hasSum = bintree_hasPathSum(b, 111);
    if (0 != hasSum) {
        logger(dbgErr, "Expected no pathsum of 111, but found it");
        FAIL_TEST;
    }

    /* cleanup */
    bintree_destroy(b);
    goto out;
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
