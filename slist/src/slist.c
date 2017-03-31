#include <stdio.h>
#include <stdlib.h>

/* Internal Node */
typedef struct node_s {
    void* data;
    struct node_s *next;
} node_t;

/* Public List */
typedef struct slist_s {
    void (*node_print)(void*);
    node_t *head;
} slist_t;

/* Public APIs */
void slist_new(slist_t *slist, void (*slist_node_print)(void*));
void slist_destroy(slist_t *slist);
void slist_print(slist_t *slist);
void slist_append(slist_t *slist, void* data);
void slist_prepend(slist_t *slist, void* data);
void slist_delete_first(slist_t *slist);
void slist_delete_last(slist_t *slist);
unsigned int slist_count(slist_t *slist);

/**
 * Print a single-linked list
 *
 * @param slist (i) slist to print 
 * @return void
 */
void slist_print(slist_t *slist) {
    int i = 0;
    node_t *cur = slist->head;
    while (cur != NULL) {
	printf("slist[%u] = ", i++);
	slist->node_print(cur->data);
	cur = cur->next;
    }
}

/**
 * Internal API to alloc and init a node
 *
 * Note this alloc's memory, need to call _slist_node_free to free
 * 
 * @param data (i) void pointer to data to store
 * @return node_t*
 */
node_t* _slist_node_alloc(void *data) {
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
void _slist_node_free(node_t *node) {
    free(node);
}

/**
 * Append a data chunk to the slist
 * 
 * @param slist (i) slist to append to
 * @param data  (i) data to append
 * @return void
 */
void slist_append(slist_t *slist, void *data) {
    node_t *cur = slist->head;
    node_t *new = NULL;

    /* alloc new node */
    new = _slist_node_alloc(data);

    /* if list is empty, simply update the head to point to new node */
    if (NULL == cur) {
	slist->head = new;
    } else {
	/* walk to last node and tack on new node */
	while (cur->next != NULL) {
	    cur = cur->next;
	}
	cur->next = new;
    }
}

/**
 * Prepend a data chunk to the slist
 *
 * @param slist (i) slist to prepend to
 * @param data  (i) data to prepend
 * @return void
 */
void slist_prepend(slist_t *slist, void *data) {
    node_t *cur = slist->head;
    node_t *new = NULL;

    /* alloc new node */
    new = _slist_node_alloc(data);

    new->next = cur;
    slist->head = new;
}

/**
 * Prepare a new slist
 * 
 * @param slist (i) slist to prepare
 * @param slist_node_print (i) fn-ptr for printing out the data in the slist
 * @return void
 */
void slist_new(slist_t *slist, void (*slist_node_print)(void*)) {
    slist->node_print = slist_node_print;
    slist->head = NULL;
}

/**
 * Destroy a slist
 *
 * @param slist (i) slist to destroy
 */
void slist_destroy(slist_t *slist)
{
    /* TODO */
}

/**
 * Delete the first element from a slist
 * 
 * @param slist (i) slist to delete first element from
 * @return void
 */
void slist_delete_first(slist_t *slist) {
    node_t *cur = slist->head;
    
    if (NULL == cur) {
	printf("Nothing to delete, list empty\n");
    } else {
	printf("Delete first node...\n");
	slist->head = cur->next;
	_slist_node_free(cur);
    }
}

/**
 * Delete the last element from a slist
 *
 * @param slist (i) slist to delete last element from
 * @return void
 */
void slist_delete_last(slist_t *slist) {
    node_t *cur = slist->head;

    if (NULL == cur) {
	printf("Nothing to delete, list empty\n");
    }
    else if (NULL == cur->next) {
	printf("Only 1 item to delete, the head\n");
	_slist_node_free(cur);
	slist->head = NULL;
    } else {
	node_t *toDelete = cur;
	node_t *prev = cur;
	/* advance toDelete to end of list, keeping prev one behind it */
	while (NULL != toDelete->next) {
	    prev = toDelete;
	    toDelete = toDelete->next;
	}
	printf("Found second to last node: ");
	slist->node_print(prev->data);
	printf("Will delete last node: ");
	slist->node_print(toDelete->data);

	_slist_node_free(toDelete);
	prev->next = NULL;

/* Alternative way to delete last node, using single 'cur' pointer */
#ifdef FALSE
	while(NULL != cur->next->next) {
	    cur = cur->next;
	}
	printf("Found second to last node: ");
	slist->node_print(cur->data);
	printf("Will delete last node: ");
	slist->node_print(cur->next->data);

	/* now sitting at next-to-last node */
	_slist_node_free(cur->next);
	cur->next = NULL;
#endif

    }
}

/**
 * Return how many nodes are in the list
 * 
 * @param slist (i) slist to count
 * @return count of nodes in list
 */
unsigned int slist_count(slist_t *slist)
{
    unsigned int cnt = 0;
    node_t *cur = slist->head;

    while (NULL != cur) {
	cnt++;
	cur = cur->next;
    }
    return cnt;
}

/**
 * Helper API to print an integer
 *
 * @param data (i) void-ptr to integer to print
 * @return void
 */
void print_integer(void *data) {
    printf("slist item: %u\n", *(unsigned int*)(data));
}

int main(int argc, char *argv[])
{
    int x = 5, y = 7, z = 8;
    slist_t slist;
    slist_new(&slist, print_integer);
    slist_prepend(&slist, &x);
    slist_prepend(&slist, &y);
    slist_prepend(&slist, &z);
    slist_print(&slist);
    printf("There are %u items in the list\n", slist_count(&slist));
    slist_print(&slist);
    slist_delete_first(&slist);
    slist_print(&slist);
    slist_delete_last(&slist);
    slist_print(&slist);

    exit(0);
}
