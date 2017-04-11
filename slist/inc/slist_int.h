#ifndef __SLIST_INT_H__
#define __SLIST_INT_H__

#define SLIST_MAGIC_IN_USE 0x1357
#define SLIST_MAX_NAME_LEN 80

/* Internal Node */
typedef struct node_s {
    void* data;
    struct node_s *next;
} node_t;


/* Public List */
typedef struct slist_s {
    int magic;
    char name[SLIST_MAX_NAME_LEN];
    node_t *head;
} slist_t;

#endif /* __SLIST_INT_H__ */

