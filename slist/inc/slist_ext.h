#ifndef __SLIST_EXT_H__
#define __SLIST_EXT_H__

typedef struct slist_s* ListPtr;

/* Public APIs */
ListPtr slist_new(const char* name);
void slist_destroy(ListPtr listp);
void slist_add_tail(ListPtr listp, void* data);
void slist_add_head(ListPtr listp, void* data);
void slist_del_tail(ListPtr listp);
void slist_del_head(ListPtr listp);
void slist_reverse(ListPtr listp);
void slist_apply_fn(ListPtr listp, void (*apply_fn)(void *));
void* slist_get_pos(ListPtr listp, int pos);
int slist_count(ListPtr listp);

#endif /* __SLIST_EXT_H__ */
