#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yem_ht.h"

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

struct yem_ht* yem_ht_init(uint64_t capacity) {
    struct yem_ht* ht = malloc(sizeof(struct yem_ht));
    ht->capacity = capacity;
    ht->length = 0;
    ht->items = calloc(capacity, sizeof(struct yem_ht_item));

    return ht;
}

void yem_ht_push(struct yem_ht* ht, int key, char* path) {
    char* key_str = malloc(11);
    snprintf(key_str, 11, "%d", key);
    uint64_t idx = hash_key(key_str) % ht->capacity;

    struct yem_ht_item* item = malloc(sizeof(struct yem_ht_item));
    item->key = key_str;
    item->path = path;
    item->next = malloc(sizeof(struct yem_ht_item));

    struct yem_ht_item* curr = &ht->items[idx];
    struct yem_ht_item* prev = &ht->items[idx];

    if (curr->key == NULL) {
        ht->items[idx] = *item;
        ++ht->length;
        return;
    }

    while (curr->key != NULL) {
        if (curr->key == key_str) {
            item->next = curr->next;
            ht->items[idx] = *item;
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    prev->next = item;
    ++ht->length;
}

struct yem_ht_item* yem_ht_get(struct yem_ht* ht, int key) {
    char* key_str = malloc(11);
    snprintf(key_str, 11, "%d", key);
    uint64_t idx = hash_key(key_str) % ht->capacity;

    struct yem_ht_item* curr = &ht->items[idx];
    while (curr->key != NULL) {
        if (strcmp(curr->key, key_str) == 0) {
            break;
        }
        curr = curr->next;
    }

    return curr;
}

void yem_ht_del(struct yem_ht* ht, char* key) {
    uint64_t idx = hash_key(key) % ht->capacity;

    struct yem_ht_item* curr = &ht->items[idx];
    struct yem_ht_item* prev = &ht->items[idx];

    if (curr->key == key) {
        ht->items[idx] = *curr->next;
        --ht->length;
        return;
    }

    while (curr->key != NULL) {
        if (curr->key == key) {
            prev->next = curr->next;
            --ht->length;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

struct yem_ht_it yem_ht_iterator(struct yem_ht* ht) {
    struct yem_ht_it it;
    it._ht = ht;
    return it;
}

/** Move the iterator to the next item. This modifies the underlying hash table
 *  so be careful of how to use this. Everytime you move the iterator to a item
 *  the item itself gets removed from the table.
 */
int yem_ht_it_next(struct yem_ht_it* it) {
    struct yem_ht* ht = it->_ht;
    if (ht->length == 0) {
        return 0;
    }

    for (int i = 0; i < ht->capacity; ++i) {
        struct yem_ht_item item = ht->items[i];
        if (item.key == NULL) {
            continue;
        }
        it->key = item.key;
        it->path = item.path;
        yem_ht_del(ht, item.key);
        return 1;
    }

    return 0;
}
