#ifndef __DLIST_INT_H__
#define __DLIST_INT_H__

#define DLIST_MAGIC_IN_USE 0x1234
#define DLIST_MAGIC_FREED  0x1235

#define DLIST_MAX_NAME_LEN 80

/* Internal doubly-linked node */
typedef struct dnode_s {
    void *data;
    struct dnode_s *prev;
    struct dnode_s *next;
} dnode_t;


/* Public list */
typedef struct dlist_s {
    int magic;
    char name[DLIST_MAX_NAME_LEN];
    dnode_t *head;
} dlist_t;

#endif /* __DLIST_INT_H__ */
