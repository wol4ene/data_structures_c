#ifndef __TRIE_EXT_H__
#define __TRIE_EXT_H__

typedef struct trie_s* TriePtr;

/* Public APIs */
TriePtr trie_create(const char *name);
void trie_destroy(TriePtr trieptr);

void trie_insert(TriePtr trieptr, char *data);
int  trie_search(TriePtr trieptr, char *data);

// int trie_count(TriePtr trieptr);

#endif /* __TRIE_EXT_H__ */

