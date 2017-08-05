#ifndef __BINTREE_INT_H__
#define __BINTREE_INT_H__

#define BINTREE_MAGIC_IN_USE 0x1235
#define BINTREE_MAGIC_FREED  0x1236

#define BINTREE_MAX_NAME_LEN 80

/* Internal binary-tree node */
typedef struct bintreenode_s {
    int data;
    struct bintreenode_s *left;
    struct bintreenode_s *right;
} bintreenode_t;


/* Public list */
typedef struct bintree_s {
    int magic;
    char name[BINTREE_MAX_NAME_LEN];
    bintreenode_t *root;
} bintree_t;

#endif /* __BINTREE_INT_H__ */
