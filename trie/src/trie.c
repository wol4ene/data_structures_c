#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "trie_ext.h"
#include "trie_int.h"
#include "logger.h"

#define MAGIC_IN_USE_CHECK(_mag_) \
    if (TRIE_MAGIC_IN_USE != _mag_) { \
        logger(dbgCrit, "Magic corrupted, expected %x, received %x", \
                TRIE_MAGIC_IN_USE, _mag_); \
        goto out; \
    } \

int letter_to_idx(char c) {
    return c - 'a';
}
char idx_to_letter(int i) {
    return i + 'a';
};


/************************************
 *    Static Helpers
 ************************************/
/**
 * Internal API to alloc and init a node
 *
 * Note this alloc's memory, need to call corresponding free func
 * 
 * @return trienode_t*
 */
static trienode_t*
_trie_node_alloc(void)
{
    trienode_t *node = (trienode_t*)malloc(sizeof(*node));
    assert(NULL != node);
    int i = 0;

    node->is_leaf = FALSE;
    node->num_matches = 0;
    for (i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

/**
 * Internal API to free a previously alloc'ed node
 *
 * @param node (i) node to free
 * @return void
 */
static void
_trie_node_free(trienode_t *node)
{
    assert(NULL != node);
    free(node);
}

/**
 * Internal API to destroy a trie
 *
 * Algorithm: walk to end node recursively, then delete it
 * 
 * @param node (i) root node
 * @return void
 */
static void
_trie_destroy(trienode_t *node)
{
    int i = 0;
    for (i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        if (NULL != node->children[i]) {
            _trie_destroy(node->children[i]);
        }
    }
    _trie_node_free(node);

    return;
}

/************************************
 *    Public APIs
 ************************************/

/**
 * Create a new trie
 *
 * Note - allocs mem for a new trie, caller must call trie_destroy()
 * 
 * @param name (i) name for trie
 * @return TriePtr
 */
TriePtr 
trie_create(const char *name)
{
    assert(NULL != name);

    TriePtr tptr = (trie_t*)malloc(sizeof(*tptr));
    assert(NULL != tptr);

    tptr->magic = TRIE_MAGIC_IN_USE;
    strcpy(tptr->name, name);
    tptr->root = _trie_node_alloc();

    return tptr;
}

/**
 * Destroy a trie
 *
 * @param tptr (i) ptr to trie to destroy
 */
void 
trie_destroy(TriePtr tptr)
{
    assert(NULL != tptr);
    MAGIC_IN_USE_CHECK(tptr->magic);

    _trie_destroy(tptr->root);
     
    /* finally, free the trie itself */
    free(tptr);
out:
    return;
}

/**
 * Insert a word into the trie
 *
 * Algorithm: for each letter in input word, check if children of current node is NULL
 *            if NULL -> cur node doesn't have this child, alloc a new code and stuff it in children
 *            if !NULL -> cur node does have this child, simply keep going
 *            advance to child node
 *            once done with all letters, mark the final node as a leaf and bump the matches
 *
 * @param tptr (i) trie to add to
 * @param data (i) word to add
 */
void
trie_insert(TriePtr tptr, char *data) 
{
    assert(NULL != tptr);
    assert(NULL != data);
    MAGIC_IN_USE_CHECK(tptr->magic);

    logger(dbgInfo, "Adding word: '%s'", data);
    int i = 0, idx = 0;
    int length = strlen(data);
    trienode_t *cur = tptr->root;

    for (i = 0; i < length; i++) {
        idx = letter_to_idx(data[i]);
        if (NULL == cur->children[idx]) {
            printf("No child for '%c', adding new node\n", data[i]);
            cur->children[idx] = _trie_node_alloc();
        } else { 
            printf("Child letter '%c' found, advancing to it\n", idx_to_letter(idx));
        }
        cur = cur->children[idx];
    }
    printf("finished adding '%s'\n", data);

    /* at end of word - need to set leaf to true and bump matches */
    cur->is_leaf = TRUE;
    cur->num_matches++;

out:
    return;
}

/**
 * Search for a word in a trie
 *
 * Algorithm:  for each letter in the input word, convert letter to index
 *             and check current node's children array for NULL.
 *             if NULL -> no entry for letter, return 0
 *             if !NULL -> walk down to this child and repeat
 *             once we've hit all the letters, simply check to see if cur node is a leaf
 *
 * @param tptr (i) trie to search
 * @param data (i) word to search for 
 * @return void
 */
int
trie_search(TriePtr tptr, char *data) 
{
    trienode_t *cur = NULL;
    int i = 0, idx = 0;
    assert(NULL != tptr);
    MAGIC_IN_USE_CHECK(tptr->magic);

    logger(dbgInfo, "Searching for word: '%s'", data);

    int length = strlen(data);
    cur = tptr->root;

    for (i = 0; i < length; i++) {
        idx = letter_to_idx(data[i]);
        if (NULL == cur->children[idx]) {
            return 0;  /* found a letter not present in trie, return 0 immediately */
        }
        cur = cur->children[idx];
        printf("Matched on '%c'\n", data[i]);
    }
out:
    return (NULL != cur && cur->is_leaf);
}
