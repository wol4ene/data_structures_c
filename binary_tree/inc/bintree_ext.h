#ifndef __BINTREE_EXT_H__
#define __BINTREE_EXT_H__

typedef struct bintree_s* BintreePtr;

/* Public APIs */
BintreePtr bintree_create(const char *name);
void bintree_destroy(BintreePtr bintreep);

void bintree_insert(BintreePtr bintreep, int data);
void bintree_remove(BintreePtr bintreep, int data);
int  bintree_search(BintreePtr bintreep, int data);
int  bintree_count(BintreePtr bintreep);

void bintree_preorder(BintreePtr bintreep);
void bintree_inorder(BintreePtr bintreep);
void bintree_postorder(BintreePtr bintreep);

#endif /* __BINTREE_EXT_H__ */

