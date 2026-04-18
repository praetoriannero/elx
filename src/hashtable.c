// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "hashtable.h"
#include "allocator.h"

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func,
                          KeyEqualFunc key_comp, HashTableFreeKey free_key,
                          HashTableFreeValue free_value) {
  HashTable* table = NULL;
  hash_table_init(table, alloc, hash_func, key_comp, free_key, free_value);

  return table;
}

void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func,
                     KeyEqualFunc key_comp, HashTableFreeKey free_key,
                     HashTableFreeValue free_value) {
  u64 first_prime = hash_primes[0];
  Vector entry_vec;
  vector_init(&entry_vec, alloc, sizeof(usize), first_prime, NULL);

  *self = (HashTable){
      .alloc = alloc,
      .comp_func = key_comp,
      .hash_func = hash_func,
      .free_key = free_key,
      .free_value = free_value,
      .entries = entry_vec,
      .prime = first_prime,
      .population = 0,
  };
}

void hash_table_add(HashTable* self, void* key, void* value) {
  u64 key_hash = self->hash_func(key);
  usize key_loc = key_hash % self->entries.size;
}

void* hash_table_remove(HashTable* self, void* key);

void* hash_table_get(HashTable* self, void* key);

void hash_table_free(HashTable* self) { allocator_free(self->alloc, self); }

void hash_table_iter_init(HashTableIter* self, HashTable* table);

void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self,
                                HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);
