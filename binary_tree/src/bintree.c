#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bintree_ext.h"
#include "bintree_int.h"
#include "logger.h"

#define MAGIC_IN_USE_CHECK(_mag_) \
    if (BINTREE_MAGIC_IN_USE != _mag_) { \
        logger(dbgCrit, "Magic corrupted, expected %x, received %x", \
                BINTREE_MAGIC_IN_USE, _mag_); \
        goto out; \
    } \

/************************************
 *    Static Helpers
 ************************************/
/**
 * Internal API to alloc and init a node
 *
 * Note this alloc's memory, need to call corresponding free func
 * 
 * @param data (i) data to store
 * @return node_t*
 */
static bintreenode_t*
_bintree_node_alloc(int data)
{
    bintreenode_t *node = (bintreenode_t*)malloc(sizeof(*node));
    assert(NULL != node);

    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/**
 * Internal API to free a previously alloc'ed node
 *
 * @param node (i) node to free
 * @return void
 */
static void
_bintree_node_free(bintreenode_t *node)
{
    assert(NULL != node);
    free(node);
}

/**
 * Internal API to destroy a binary tree
 *
 * Algorithm: post-order traversal, then free the node
 * 
 * @param node (i) rot node
 * @return void
 */
static void
_bintree_destroy(bintreenode_t *node)
{
    if (NULL == node) {
        return;
    }
    _bintree_destroy(node->left);
    _bintree_destroy(node->right);

    logger(dbgInfo, "Destroying node %i", node->data);
    _bintree_node_free(node);

    return;
}

/**
 * Internal helper to add node to tree
 *
 * Algorithm: Recursively walk left or right subtree, looking for NULL node,
 *            which is the base case.  Return a new node to the caller,
 *            which will set new node's parent link to this new node
 *
 * @param node (i) ptr to current node while traversing tree
 * @param data (i) data to insert
 * @return ptr to a node
 */
static bintreenode_t*
_insert_node(bintreenode_t *node, int data)
{
    if (NULL == node) {
        logger(dbgInfo, "Hit leaf node, adding %i", data);
        return _bintree_node_alloc(data);
    }

    if (data < node->data) {
        logger(dbgInfo, "Cur node %i, data %i, going left", node->data, data);
        node->left = _insert_node(node->left, data);
    } else {
        logger(dbgInfo, "Cur node %i, data %i, going right", node->data, data);
        node->right = _insert_node(node->right, data);
    }

    return node;
}

/**
 * Internal helper to recursively delete a node in a binary tree
 * 
 * Algorithm: 
 * Search for the node to delete. Do recursively, which will keep track of 'parent'
 * node once we find the node to delete. this will be important later
 *
 * Once node is found:
 *    if leaf node:
 *       simply delete node and return NULL, which sets parent to NULL
 *    if only has right subtree:
 *       delete node and return ptr to right subtree, which will link parent to right subtree
 *    if only has left subtree:
 *       delete node and return ptr to left subtree, which will link parent to left subtree
 *    if has both subtrees:
 *       find sucessor node - min node of right subtree
 *       copy successor node data value into current node
 *       recursively delete right subtree of current node, looking for the now-duplicate
 *          data value, which will be guaranteed to be a leaf or right-subtree only case      
 */
static bintreenode_t*
_bintree_remove(bintreenode_t *root, int data) {
    if (NULL == root) {
        printf("Data %i not found in tree\n", data);
        return root;
    }

    if (data < root->data) {
        root->left = _bintree_remove(root->left, data);
    } else if (data > root->data) {
        root->right = _bintree_remove(root->right, data);
    } else {
        if (NULL == root->left && NULL == root->right) {
            _bintree_node_free(root);
            return NULL;
        }
        else if (NULL == root->left && NULL != root->right) {
            bintreenode_t *tmp = root->right;
            _bintree_node_free(root);
            return tmp;
        }
        else if (NULL != root->left && NULL == root->right) {
            bintreenode_t *tmp = root->left;
            _bintree_node_free(root);
            return tmp;
        } else {
            bintreenode_t *min_node = root->right;
            while (NULL != min_node->left) {
                min_node = min_node->left;
            };
            root->data = min_node->data;
            root->right = _bintree_remove(root->right, root->data);
        }
    }
    return root;
}

/** 
 * Internal API to search a binary tree for a datum
 *
 * Algorithm: Simliar to a lookup, proceed left or right until either
 * no match is found (hit a NULL on a leaf) or match is found
 *
 * @param node (i) root node
 * @param data (i) data to search for
 * @return 1 if found, 0 if not found
 */
int
_bintree_search(bintreenode_t *node, int data) { 
    if (NULL == node) {
        logger(dbgInfo, "Did not find node %i", data);
        return 0;
    }
    if (data == node->data) {
        logger(dbgInfo, "Found node %i", data);
        return 1;
    }
    if (data < node->data) {
        return _bintree_search(node->left, data);
    } else {
        return _bintree_search(node->right, data);
    }
}

/**
 * Internal API to count tree nodes
 *
 * Algorithm: For each non-null node visited, return 
 *            1 + count in left subtree + count in right subtree
 *
 * @param node (i) root node
 * @return count of nodes in tree
 */
static int
_bintree_count(bintreenode_t *node) {
    if (NULL == node) {
        return 0;
    }
    return (1 + 
            _bintree_count(node->left) + 
            _bintree_count(node->right));
};

/**
 * Internal API to perform preorder traversal
 * 
 * Algorithm: 
 *   visit root, then left tree, then right tree
 *
 * @param node (i) root node
 * @return void
 */
static void
_bintree_preorder(bintreenode_t *node)
{
    if (NULL == node) {
        return;
    }
    logger(dbgInfo, "Preorder: %i, ", node->data);
    _bintree_preorder(node->left);
    _bintree_preorder(node->right);
}

/**
 * Internal API to perform inorder traversal
 * 
 * Algorithm:
 *   visit left tree, then root, then right tree
 *
 * @param node (i) root node
 * @return void
 */
static void
_bintree_inorder(bintreenode_t *node) {
    if (NULL == node) {
        return;
    }
    _bintree_inorder(node->left);
    logger(dbgInfo, "Inorder: %i, ", node->data);
    _bintree_inorder(node->right);
}

/**
 * Internal API to perform postorder traversal
 * 
 * Algorithm:
 *   visit left tree, then right tree, then root
 *
 * @param node (i) root node
 * @return void
 */
static void
_bintree_postorder(bintreenode_t *node) {
    if (NULL == node) {
        return;
    }
    _bintree_postorder(node->left);
    _bintree_postorder(node->right);
    logger(dbgInfo, "Postorder: %i, ", node->data);
}

/**
 * Helper to find the max depth of a binary tree
 *
 * Algorithm: Recur down left and right subtrees, adding one for each
 *            level we've gone down.  
 *
 * @param node (i) root node
 * @return max-depth
 */
static int
_bintree_maxdepth(bintreenode_t *node)
{
    if (NULL == node) {
        return 0;
    }
    
    int depth_left = _bintree_maxdepth(node->left) + 1;
    int depth_right = _bintree_maxdepth(node->right) + 1;

    return (depth_left > depth_right ? depth_left : depth_right);
}

/**
 * Helper to find if a path exists in the tree that has the given sum
 *
 * Algorithm: recur down all paths, and at each node, subtract current
 *            node's value from sum.  Once we reach a NULL node, if the
 *            sum has reached zero - return TRUE
 *
 * @param node (i) root node
 * @param sum  (i) sum to search for
 * @return 1 if sum exists in a path, 0 else
 */
static int
_bintree_hasPathSum(bintreenode_t *node, int sum)
{
    if (NULL == node) {
        return (0 == sum);
    }
    int new_sum = sum - node->data;
    int left_has_sum = _bintree_hasPathSum(node->left, new_sum);
    int right_has_sum = _bintree_hasPathSum(node->right, new_sum);
    return (left_has_sum || right_has_sum);
}

/************************************
 *    Public APIs
 ************************************/

/**
 * Create a new binary tree
 *
 * Note - allocs mem for a new tree, caller must call bintree_destroy()
 * 
 * @param name (i) name for binary tree
 * @return BintreePtr
 */
BintreePtr 
bintree_create(const char *name)
{
    assert(NULL != name);

    BintreePtr bintreep = (bintree_t*)malloc(sizeof(*bintreep));
    assert(NULL != bintreep);

    bintreep->magic = BINTREE_MAGIC_IN_USE;
    strcpy(bintreep->name, name);
    bintreep->root = NULL;

    return bintreep;
}

/**
 * Destroy a binary tree
 *
 * @param bintreep (i) ptr to binary tree to destroy
 */
void 
bintree_destroy(BintreePtr bintreep)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    _bintree_destroy(bintreep->root);
     
    /* finally, free the bintree itself */
    free(bintreep);
out:
    return;
}

/**
 * Insert a node to the binary tree
 *
 * @param bintreep (i) binary tree to add to
 * @param data     (i) data to add
 */
void
bintree_insert(BintreePtr bintreep, int data) 
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    logger(dbgInfo, "Adding data: %i", data);
    bintreep->root = _insert_node(bintreep->root, data);
out:
    return;
}

/**
 * Remove a node from a binary tree
 *
 * @param bintreep (i) binary tree to remove from
 * @param data     (i) data value to remove
 * @return void
 */
void 
bintree_remove(BintreePtr bintreep, int data)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    logger(dbgInfo, "Removing data: %i", data);
    _bintree_remove(bintreep->root, data);
out:
    return;
}

/**
 * Search for a node in a binary tree
 *
 * @param bintreep (i) binary tree to search
 * @param data     (i) data to search for 
 * @return void
 */
int
bintree_search(BintreePtr bintreep, int data) 
{
    int found = 0;

    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    logger(dbgInfo, "Searching for data: %i", data);
    found = _bintree_search(bintreep->root, data);

out:
    return found;

}

/**
 * Return how many nodes are in the binary tree
 *
 * @param bintreep (i) binary tree to count
 * @return count of nodes in binary tree
 */
int 
bintree_count(BintreePtr bintreep)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    int cnt = _bintree_count(bintreep->root);
out:
    return cnt;
}

/**
 * Find the min-value in a binary tree
 * 
 * @param bintreep (i) binary tree
 * @return minimum value
 */
int
bintree_minvalue(BintreePtr bintreep)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    bintreenode_t *cur = bintreep->root;
    while (NULL != cur->left) {
        cur = cur->left;
    }
out:
    return cur->data;
}

/**
 * Find the max-value in a binary tree
 * 
 * @param bintreep (i) binary tree
 * @return maximum value
 */
int
bintree_maxvalue(BintreePtr bintreep)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    bintreenode_t *cur = bintreep->root;
    while (NULL != cur->right) {
        cur = cur->right;
    }
out:
    return cur->data;
}


/**
 * Find the max-depth of a binary tree
 *
 * @param bintreep (i) binary tree
 * @return max depth
 */
int
bintree_maxdepth(BintreePtr bintreep)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    int max_depth = _bintree_maxdepth(bintreep->root);
out:
    return max_depth;
}

/**
 * Check if a given path-sum exists in a tree
 *
 * @param bintreep (i) binary tree
 * @param sum      (i) sum
 * @return 1 if sum exists for a path, 0 else
 */
int
bintree_hasPathSum(BintreePtr bintreep, int sum)
{
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    int has_sum = _bintree_hasPathSum(bintreep->root, sum);
out:
    return has_sum;
}

/**
 * Perform pre-order traversal of binary tree
 *     visits parent, then left tree, then right tree
 *
 * @param bintreep (i) binary tree to traverse 
 * @return void
 */
void
bintree_preorder(BintreePtr bintreep) {
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    _bintree_preorder(bintreep->root);
out:
    return;
}

/**
 * Perform in-order traversal of binary tree
 *    visits left tree, then parent, then right tree
 *
 * @param bintreep (i) binary tree to traverse 
 * @return void
 */
void
bintree_inorder(BintreePtr bintreep) {
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    _bintree_inorder(bintreep->root);
out:
    return;
}

/**
 * Perform post-order traversal of binary tree
 *     visits left tree, then right tree, then parent
 * 
 * @param bintreep (i) binary tree to traverse 
 * @return void
 */
void
bintree_postorder(BintreePtr bintreep) {
    assert(NULL != bintreep);
    MAGIC_IN_USE_CHECK(bintreep->magic);

    _bintree_postorder(bintreep->root);
out:
    return;
}
