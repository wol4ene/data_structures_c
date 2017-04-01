#ifndef __SLIST_INT_H__
#define __SLIST_INT_H__

/* Internal Node */
typedef struct node_s {
    void* data;
    struct node_s *next;
} node_t;

#define SLIST_MAX_NAME_LEN 80

/* Public List */
typedef struct slist_s {
    char name[SLIST_MAX_NAME_LEN];
    node_t *head;
} slist_t;

#endif /* __SLIST_INT_H__ */

