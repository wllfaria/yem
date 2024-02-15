#pragma once
#include <stdint.h>

struct yem_ht {
    struct yem_ht_item* items;
    uint64_t capacity;
    uint64_t length;
};

struct yem_ht_item {
    char* key;
    char* path;
    struct yem_ht_item* next;
};

struct yem_ht_it {
    char* key;
    char* path;

    /** Hash table reference to be used internally by the iterator. Please don't
     * use this directly */
    struct yem_ht* _ht;
};

struct yem_ht* yem_ht_init(uint64_t capacity);
struct yem_ht_item* yem_ht_get(struct yem_ht* ht, int key);
struct yem_ht_it yem_ht_iterator(struct yem_ht* ht);
void yem_ht_push(struct yem_ht* ht, int key, char* path);
void yem_ht_push_str(struct yem_ht* ht, char* key, char* path);
int yem_ht_it_next(struct yem_ht_it* it);
