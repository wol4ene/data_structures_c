#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "slist_ext.h"
#include "slist_int.h"
#include "logger.h"

/************************************
 *    Static Helpers
 ************************************/
/**
 * Internal API to alloc and init a node
 *
 * Note this alloc's memory, need to call _slist_node_free to free
 * 
 * @param data (i) void pointer to data to store
 * @return node_t*
 */
static node_t* 
_slist_node_alloc(void *data)
{
    assert(NULL != data);
    node_t *new = NULL;
    new = malloc(sizeof(*new));
    new->data = data;
    new->next = NULL;
    return new;
}

/**
 * Internal API to free a previously alloc'ed node
 *
 * @param node (i) node to free
 * @return void
 */
static void 
_slist_node_free(node_t *node)
{
    assert(NULL != node);
    free(node);
}


/************************************
 *    Public APIs
 ************************************/

/**
 * Prepare a new slist
 *
 * Note - allocs mem for a new list, caller must call slist_destroy()
 * 
 * @param name (i) name for list
 * @return ListPtr
 */
ListPtr 
slist_new(const char *name)
{
    assert(NULL != name);

    slist_t *tmp = (slist_t*)malloc(sizeof(slist_t));
    tmp->head = NULL;
    strncpy(tmp->name, name, sizeof(tmp->name));
    return tmp;
}

/**
 * Destroy a slist
 *
 * @param listp (i) list to destroy
 */
void 
slist_destroy(ListPtr listp)
{
    assert(NULL != listp);

    node_t *cur = listp->head;
    node_t *next = NULL;

    /* walk all nodes in list, destroying each node as we go */
    while (NULL != cur) {
	next = cur->next;
	_slist_node_free(cur);
	cur = next;
    }

    /* finally, destroy the slist itself */
    free(listp);
    listp = NULL;
}

/**
 * Append a node to the slist
 * 
 * @param listp (i) list to append to
 * @param data  (i) data to append
 * @return void
 */
void 
slist_add_tail(ListPtr listp, void *data)
{
    assert(NULL != listp);
    assert(NULL != data);

    node_t *cur = listp->head;
    node_t *new = NULL;

    /* alloc new node */
    new = _slist_node_alloc(data);

    /* if list is empty, simply update the head to point to new node */
    if (NULL == cur) {
	listp->head = new;
    } else {
	/* walk to last node and tack on new node */
	while (NULL != cur->next) {
	    cur = cur->next;
	}
	cur->next = new;
    }
}

/**
 * Prepend a node to the slist
 *
 * @param listp (i) list to prepend to
 * @param data  (i) data to prepend
 * @return void
 */
void 
slist_add_head(ListPtr listp, void *data)
{
    assert(NULL != listp);
    assert(NULL != data);

    node_t *cur = listp->head;
    node_t *new = NULL;

    /* alloc new node */
    new = _slist_node_alloc(data);

    new->next = cur;
    listp->head = new;
}

/**
 * Delete the last node from a slist
 *
 * @param listp (i) list to delete last node from
 * @return void
 */
void 
slist_del_tail(ListPtr listp)
{
    assert(NULL != listp);

    node_t *cur = listp->head;

    if (NULL == cur) {
        logger(dbgWarn, "Nothing to delete, list empty");
    }
    else if (NULL == cur->next) {
        logger(dbgWarn, "Only 1 item to delete, the head");
	_slist_node_free(cur);
	listp->head = NULL;
    } else {
	node_t *toDelete = cur;
	node_t *prev = cur;
	/* advance toDelete to end of list, keeping prev one behind it */
	while (NULL != toDelete->next) {
	    prev = toDelete;
	    toDelete = toDelete->next;
	}
	_slist_node_free(toDelete);
	prev->next = NULL;

/* Alternative way to delete last node, using single 'cur' pointer */
#ifdef FALSE
	while(NULL != cur->next->next) {
	    cur = cur->next;
	}
	/* now sitting at next-to-last node */
	_slist_node_free(cur->next);
	cur->next = NULL;
#endif
    }
}

/**
 * Delete the first node from a slist
 * 
 * @param listp (i) list to delete first node from
 * @return void
 */
void 
slist_del_head(ListPtr listp)
{
    assert(NULL != listp);

    node_t *cur = listp->head;
    
    if (NULL == cur) {
        logger(dbgWarn, "Nothing to delete, list empty");
    } else {
        logger(dbgWarn, "Delete first node");
	listp->head = cur->next;
	_slist_node_free(cur);
    }
}

/**
 * Reverse the list
 *
 * @param listp (i) list to reverse
 * @return void
 */
void 
slist_reverse(ListPtr listp)
{
    assert(NULL != listp);

    node_t *prev = NULL;
    node_t *next = NULL;
    node_t *cur = listp->head;

    while (cur != NULL) {
	next = cur->next;   /* save the next node */
	cur->next = prev;   /* point cur node back to prev */
	prev = cur;	    /* march prev forward */
	cur = next;	    /* march cur forward */
    }

    /* finally, update the list's head pointer */
    listp->head = prev;
}

/**
 * Iterate the list and call apply_fn for each node
 *
 * @param listp (i) list to iterate over
 * @param apply_fn (i) fn-ptr to call for each node
 * @return void
 */
void 
slist_apply_fn(ListPtr listp, void (*apply_fn)(void *))
{
    assert(NULL != listp);
    assert(NULL != apply_fn);

    node_t *cur = listp->head;

    /* walk all nodes in list */
    while (NULL != cur) {
	apply_fn(cur->data);
	cur = cur->next;
    }
}

/**
 * Return the data at the 'pos' node, but do not destroy the node 
 *
 * Note - uses 0-based index.  So 'pos=0' will return the head of the list.
 *
 * @param listp (i) list to get from
 * @param pos   (i) position from which to get
 * @return data value or NULL if list doesn't contain 'pos' elements
 */
void*
slist_get_pos(ListPtr listp, int pos)
{
    assert(NULL != listp);
    assert(0 <= pos);

    void *ret = NULL;
    int cur_pos = 0;
    node_t *cur = listp->head;

    while (NULL != cur) {
	if (cur_pos == pos) {
	    ret = cur->data;
	    break;	
	}
	cur_pos++;
	cur = cur->next;
    }
    return ret;
}

/**
 * Return how many nodes are in the list
 * 
 * @param listp (i) list to count
 * @return count of nodes in list
 */
int 
slist_count(ListPtr listp)
{
    assert(NULL != listp);

    int cnt = 0;
    node_t *cur = listp->head;

    /* walk all nodes in list */
    while (NULL != cur) {
	cnt++;
	cur = cur->next;
    }
    return cnt;
}

