#ifndef __TRIE_INT_H__
#define __TRIE_INT_H__

#define TRIE_MAGIC_IN_USE 0x1236
#define TRIE_MAGIC_FREED  0x1237

#define TRIE_MAX_NAME_LEN 80
#define TRIE_ALPHABET_SIZE 26  /* this example only uses 26 lowercase letters */

typedef enum {FALSE, TRUE} bool;

/* Internal trie-tree node */
typedef struct trienode_s {
    bool is_leaf;
    int num_matches;
    struct trienode_s* children[TRIE_ALPHABET_SIZE];
} trienode_t;


/* Public list */
typedef struct trie_s {
    int magic;
    char name[TRIE_MAX_NAME_LEN];
    trienode_t *root;
} trie_t;

#endif /* __TRIE_INT_H__ */
