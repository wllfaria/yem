#pragma once
#include <stdint.h>

struct yem_ht {
    struct yem_ht_item* items;
    uint64_t capacity;
    uint64_t length;
};

struct yem_ht_item {
    char* key;
    int value;
    struct yem_ht_item* next;
};

struct yem_ht* yem_ht_init(uint64_t capacity);
void yem_ht_push(struct yem_ht* ht, char* key, int value);
struct yem_ht_item* yem_ht_get(struct yem_ht* ht, char* key);
