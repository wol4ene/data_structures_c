#ifndef __DLIST_INT_H__
#define __DLIST_INT_H__

/* Internal doubly-linked node */
typedef struct dnode_s {
    void *data;
    struct dnode_s *prev;
    struct dnode_s *next;
} dnode_t;

#define DLIST_MAX_NAME_LEN 80

/* Public list */
typedef struct dlist_s {
    char name[DLIST_MAX_NAME_LEN];
    dnode_t *head;
} dlist_t;

#endif /* __DLIST_INT_H__ */
