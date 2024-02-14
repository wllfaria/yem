#include <stdio.h>
#include <stdlib.h>

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

struct yem_ht_item {
    char* key;
    int value;
    struct yem_ht_item* next;
};

struct yem_ht {
    struct yem_ht_item* items;
    uint64_t capacity;
    uint64_t length;
};

struct yem_ht* yem_ht_init(uint64_t capacity) {
    struct yem_ht* ht = malloc(sizeof(struct yem_ht));
    ht->capacity = capacity;
    ht->length = 0;
    ht->items = calloc(capacity, sizeof(struct yem_ht_item));

    return ht;
}

void yem_ht_push(struct yem_ht* ht, char* key, int value) {
    uint64_t idx = hash_key(key) % ht->capacity;

    struct yem_ht_item* item = malloc(sizeof(struct yem_ht_item));
    item->key = key;
    item->value = value;
    item->next = malloc(sizeof(struct yem_ht_item));

    struct yem_ht_item existing = ht->items[idx];

    if (existing.key != NULL) {
        struct yem_ht_item* curr = &ht->items[idx];
        while (curr->next->key != NULL) {
            curr = curr->next;
        }
        curr->next = item;
        return;
    }

    ht->items[idx] = *item;
    ++ht->length;
}

struct yem_ht_item* yem_ht_get(struct yem_ht* ht, char* key) {
    uint64_t idx = hash_key(key) % ht->capacity;

    struct yem_ht_item* curr = &ht->items[idx];
    while (curr->key != NULL) {
        if (curr->key == key) {
            break;
        }
        curr = curr->next;
    }

    return curr;
}
