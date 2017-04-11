#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dlist_ext.h"
#include "dlist_int.h"
#include "logger.h"

/************************************
 *    Static Helpers
 ************************************/
/**
 * Internal API to alloc and init a node
 *
 * Note this alloc's memory, need to call _dlist_node_free to free
 * 
 * @param data (i) void pointer to data to store
 * @return node_t*
 */
static dnode_t*
_dlist_node_alloc(void *data)
{
    dnode_t *node = (dnode_t*)malloc(sizeof(dnode_t));
    assert(NULL != node);

    node->data = data;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

/**
 * Internal API to free a previously alloc'ed node
 *
 * @param node (i) node to free
 * @return void
 */
static void
_dlist_node_free(dnode_t *node)
{
    assert(NULL != node);
    logger(dbgInfo, "Freeing %i", *(int*)node->data);
    free(node);
}

/************************************
 *    Public APIs
 ************************************/

/**
 * Prepare a new dlist
 *
 * Note - allocs mem for a new list, caller must call dlist_destroy()
 * 
 * @param name (i) name for list
 * @return ListPtr
 */
DListPtr 
dlist_new(const char *name)
{
    assert(NULL != name);
    DListPtr listp = (dlist_t*)malloc(sizeof(dlist_t));
    assert(NULL != listp);

    strcpy(listp->name, name);
    listp->head = NULL;
    listp->magic = DLIST_MAGIC_IN_USE;
    return listp;
}

/**
 * Destroy a dlist
 *
 * @param listp (i) list to destroy
 */
void 
dlist_destroy(DListPtr listp)
{
    if (NULL == listp) {
        logger(dbgWarn, "No list provided, nothing to destroy");
        goto out;
    }
    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to destroy");
        goto out;
    }
     
    dnode_t *cur = listp->head;
    dnode_t *next = listp->head;

    /* walk the list, freeing each node */
    while (NULL != cur) {
        next = cur->next; 
        _dlist_node_free(cur);
        cur = next;
    }
    /* finally, free the dlist itself */
    free(listp);
out:
    return;
}

/**
 * Append a node to the dlist
 * 
 * @param listp (i) list to append to
 * @param data  (i) data to append
 * @return void
 */
void 
dlist_add_tail(DListPtr listp, void *data)
{
    logger(dbgInfo, "Adding tail %i", *(int*)data);

    assert(NULL != listp);
    assert(NULL != data);
    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return;
    }

    dnode_t *cur = listp->head;
    
    /* alloc a new node */
    dnode_t *new = _dlist_node_alloc(data);

    /* if list is empty, simply add to head */
    if (NULL == cur) {
        logger(dbgInfo, "Empty list, adding to head");
        listp->head = new;
    } else {
        /* walk to the final node */
        while (NULL != cur->next) {
            cur = cur->next;
        }

        /* tack on new node */
        new->prev = cur;
        cur->next = new;
    }

}

/**
 * Prepend a node to the dlist
 *
 * @param listp (i) list to prepend to
 * @param data  (i) data to prepend
 * @return void
 */
void 
dlist_add_head(DListPtr listp, void *data)
{
    logger(dbgInfo, "Adding head %i", *(int*)data);
    assert(NULL != listp);
    assert(NULL != data);
    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return;
    }

    /* alloc a new node */
    dnode_t *new = _dlist_node_alloc(data);

    /* if list has at least one node, link cur head with new node */
    if (NULL != listp->head) {
        listp->head->prev = new;
        new->next = listp->head;
    } 
    /* point head to the new node */
    listp->head = new;
}

/**
 * Delete the last node from a dlist
 *
 * @param listp (i) list to delete last node from
 * @return void
 */
void 
dlist_del_tail(DListPtr listp)
{
    logger(dbgInfo, "Deleting tail");
    assert(NULL != listp);
    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return;
    }

    dnode_t *cur = listp->head;
    
    /* if empty, nothing to do */
    if (NULL == cur) {
        logger(dbgWarn, "Empty list, nothing to del");
    } else if (NULL == cur->next) { /* only a single item in list */
        _dlist_node_free(cur);
        listp->head = NULL;
    } else { /* more than 1 node */

        /* walk to final node */
        while (NULL != cur->next) {
            cur = cur->next;
        }
        logger(dbgWarn, "Settled on node %i to del", *(int*)cur->data);
        /* set prev's node next to null */
        cur->prev->next = NULL;

        /* finally, delete cur node */
        _dlist_node_free(cur);
    }
}

/**
 * Delete the first node from a dlist
 * 
 * @param listp (i) list to delete first node from
 * @return void
 */
void 
dlist_del_head(DListPtr listp)
{
    logger(dbgInfo, "Deleting head");
    assert(NULL != listp);
    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return;
    }

    dnode_t *cur = listp->head;

    /* if empty, nothing to do */
    if (NULL == cur) {
        logger(dbgWarn, "Empty list, nothing to del");
    } else if (NULL == cur->next) { /* only a single item in list */
        _dlist_node_free(cur);
        listp->head = NULL;
    } else { /* more than 1 item */
        listp->head = cur->next;
        listp->head->prev = NULL;
        _dlist_node_free(cur);
    }
}

/**
 * Reverse the list
 *
 * @param listp (i) list to reverse
 * @return void
 */
void 
dlist_reverse(DListPtr listp)
{
    logger(dbgInfo, "Reversing");
    assert(NULL != listp);
    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return;
    }
    if (NULL == listp->head ||
        NULL == listp->head->next) {
        logger(dbgInfo, "0 or 1 elements in list, nothing to reverse");
        return;
    }

    dnode_t *tmp = NULL;
    dnode_t *cur  = listp->head;

    while (NULL != cur) {
        tmp = cur->prev;
        cur->prev = cur->next;
        cur->next = tmp;
        cur = cur->prev;
    }

    /* finally update the head pointer */
    listp->head = tmp->prev;
}

/* Alternative methods for doing dlist_reverse above */
#ifdef FALSE
    
    // Method - use a 'prev' and 'next' ptr, like a single linked list
    dnode_t *prev = NULL;
    dnode_t *next = NULL;
    dnode_t *cur  = listp->head;

    while (NULL != cur) {
        next = cur->next;
        cur->next = prev;
        cur->prev = next;
        prev = cur;
        cur = next;
    }

    /* finally update the head pointer */
    listp->head = prev;
#endif

/**
 * Iterate the list and call apply_fn for each node
 *
 * @param listp (i) list to iterate over
 * @param apply_fn (i) fn-ptr to call for each node
 * @return void
 */
void 
dlist_apply_fn(DListPtr listp, void (*apply_fn)(void *))
{
    assert(NULL != listp);
    assert(NULL != apply_fn);

    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return;
    }

    dnode_t *cur = listp->head;

    /* walk each node of the list */
    while (NULL != cur) {
        apply_fn(cur->data); 
        cur = cur->next;
    }
}

/**
 * Return the data at the 'pos' node, but do not destroy the node
 * 
 * Note - uses 0-based index. So 'pos=0' will return the head of the list
 *
 * @param listp (i) list to get from
 * @param pos   (i) position from which to get
 * @return data value or NULL if list doesn't contain 'pos' elements
 */
void* 
dlist_get_pos(DListPtr listp, int pos)
{
    assert(NULL != listp);
    assert(0 <= pos);

    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return NULL;
    }

    dnode_t *cur = listp->head;
    void *ret = NULL;
    int cur_pos = 0;

    /* walk each node in the list */
    while (NULL != cur) {
        if (cur_pos++ == pos) {
            ret = cur->data;
            break;
        }
        cur = cur->next;
    }
    return ret;
}

/**
 * Return how many nodes are in the list
*
* @param listp (i) list to count
* @reutrn count of nodes in list
*/
int 
dlist_count(DListPtr listp)
{
    assert(NULL != listp);

    if (DLIST_MAGIC_IN_USE != listp->magic) {
        logger(dbgCrit, "Magic corrupted, no dlist to operate on");
        return 0;
    }

    dnode_t *cur = listp->head;
    int cnt = 0;

    while(NULL != cur) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

