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
 * Destroys in post-order
 * 
 * @param node (i) rot node
 * @return void
 */
static void
_bintree_destroy(bintreenode_t *node) {
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
 * @param node (i) ptr to current node while traversing tree
 * @param data (i) data to insert
 * @return ptr to a node
 */
static bintreenode_t*
_insert_node(bintreenode_t *node, int data) {

    /* base case - hit a leaf, return the new node */
    if (NULL == node) {
        logger(dbgInfo, "Hit leaf node, adding %i", data);
        return _bintree_node_alloc(data);
    }

    /* recursive case - traverse down left or right subtree */
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
 * Simliar to a lookup, proceed left or right until either
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
 * Could use any traversal method
 *
 * @param node (i) root node
 * @parma cnt  (i) store counter here
 * @return void
 */
static void
_bintree_count(bintreenode_t *node, int *cnt) {
    if (NULL == node) {
        return;
    }
    (*cnt)++;
    _bintree_count(node->left, cnt);
    _bintree_count(node->right, cnt);
};

/**
 * Internal API to perform preorder traversal
 * 
 *   First visit root, then left tree, then right tree
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
 *   First visit left tree, then root, then right tree
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
 *   First visit left tree, then right tree, then root
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

    int cnt = 0;
    _bintree_count(bintreep->root, &cnt);
out:
    return cnt;
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
