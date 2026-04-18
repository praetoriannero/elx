// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "hashtable.h"

HashTable* hashtable_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc hash_comp);

void hashtable_init(HashTable* table, Allocator* alloc);

void* hashtable_add(HashTable* table, void* key, void* value);

void* hashtable_remove(HashTable* table, void* key);

void* hashtable_get(HashTable* table, void* key);
