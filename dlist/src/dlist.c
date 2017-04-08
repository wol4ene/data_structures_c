#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dlist_ext.h"
#include "dlist_int.h"
#include "logger.h"

static dnode_t*
_alloc_dlist_node(void *data)
{
    dnode_t *node = (dnode_t*)malloc(sizeof(dnode_t));
    assert(NULL != node);

    node->data = data;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

static void
_free_dlist_node(dnode_t *node)
{
    assert(NULL != node);
    logger(dbgInfo, "Freeing %i", *(int*)node->data);
    free(node);
}

DListPtr 
dlist_new(const char *name)
{
    assert(NULL != name);
    DListPtr listp = (dlist_t*)malloc(sizeof(dlist_t));
    assert(NULL != listp);

    strcpy(listp->name, name);
    listp->head = NULL;
    return listp;
}

void dlist_destroy(DListPtr listp)
{
    if (NULL == listp) {
        logger(dbgWarn, "No list provided, nothing to destroy");
    }
     
    dnode_t *cur = listp->head;
    dnode_t *next = listp->head;

    /* walk the list, freeing each node */
    while (NULL != cur) {
        next = cur->next; 
        _free_dlist_node(cur);
        cur = next;
    }
    /* finally, free the dlist itself */
    free(listp);
}

void 
dlist_add_tail(DListPtr listp, void *data)
{
    logger(dbgInfo, "Adding tail %i", *(int*)data);
    assert(NULL != listp);
    assert(NULL != data);
    dnode_t *cur = listp->head;
    
    /* alloc a new node */
    dnode_t *new = _alloc_dlist_node(data);

    /* if no nodes in list, simply add to head */
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

void 
dlist_add_head(DListPtr listp, void *data)
{
    logger(dbgInfo, "Adding head %i", *(int*)data);
    assert(NULL != listp);
    assert(NULL != data);

    /* alloc a new node */
    dnode_t *new = _alloc_dlist_node(data);

    /* if list has at least one node, link cur head with new node */
    if (NULL != listp->head) {
        listp->head->prev = new;
        new->next = listp->head;
    } 
    /* point head to the new node */
    listp->head = new;
}

void 
dlist_del_tail(DListPtr listp)
{
    logger(dbgInfo, "Deleting tail");
    assert(NULL != listp);
    dnode_t *cur = listp->head;
    
    /* if empty, nothing to do */
    if (NULL == cur) {
        logger(dbgWarn, "Empty list, nothing to del");
    } else if (NULL == cur->next) { /* only a single item in list */
        _free_dlist_node(cur);
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
        _free_dlist_node(cur);
    }
}

void 
dlist_del_head(DListPtr listp)
{
    logger(dbgInfo, "Deleting head");
    assert(NULL != listp);
    dnode_t *cur = listp->head;

    /* if empty, nothing to do */
    if (NULL == cur) {
        logger(dbgWarn, "Empty list, nothing to del");
    } else if (NULL == cur->next) { /* only a single item in list */
        _free_dlist_node(cur);
        listp->head = NULL;
    } else { /* more than 1 item */
        listp->head = cur->next;
        listp->head->prev = NULL;
        _free_dlist_node(cur);
    }
}

void 
dlist_reverse(DListPtr listp)
{
    assert(NULL != listp);
    logger(dbgInfo, "Reversing");

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

void 
dlist_apply_fn(DListPtr listp, void (*apply_fn)(void *))
{
    assert(NULL != listp);
    assert(NULL != apply_fn);

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

int 
dlist_count(DListPtr listp)
{
    assert(NULL != listp);
    dnode_t *cur = listp->head;
    int cnt = 0;

    while(NULL != cur) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

