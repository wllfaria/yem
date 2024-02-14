#include <stdint.h>

struct yem_ht* yem_ht_init(uint64_t capacity);
void yem_ht_push(struct yem_ht* ht, char* key, int value);
struct yem_ht_item* yem_ht_get(struct yem_ht* ht, char* key);
