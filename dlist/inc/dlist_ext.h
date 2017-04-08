#ifndef __DLIST_EXT_H__
#define __DLIST_EXT_H__

typedef struct dlist_s* DListPtr;

/* Public APIs */
DListPtr dlist_new(const char *name);
void dlist_destroy(DListPtr listp);
void dlist_add_tail(DListPtr listp, void *data);
void dlist_add_head(DListPtr listp, void *data);
void dlist_del_tail(DListPtr listp);
void dlist_del_head(DListPtr listp);
void dlist_reverse(DListPtr listp);
void dlist_apply_fn(DListPtr listp, void (*apply_fn)(void *));
void* dlist_get_pos(DListPtr listp, int pos);
int dlist_count(DListPtr listp);

#endif /* __DLIST_EXT_H__ */

