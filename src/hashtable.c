// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "hashtable.h"
#include "allocator.h"

constexpr f64 MAX_HASH_TABLE_LOAD = 0.7;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                          FreeValueFunc free_value) {
  HashTable* table = NULL;
  hash_table_init(table, alloc, hash_func, key_comp, free_key, free_value);

  return table;
}

void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                     FreeValueFunc free_value) {
  usize prime_idx = hash_primes[0];
  Vector entry_vec;
  vector_init(&entry_vec, alloc, sizeof(usize), prime_idx, NULL);
  vector_reserve(&self->entries, self->prime_idx);

  *self = (HashTable){
      .alloc = alloc,
      .comp_func = key_comp,
      .hash_func = hash_func,
      .free_key = free_key,
      .free_value = free_value,
      .entries = entry_vec,
      .prime_idx = prime_idx,
      .population = 0,
  };
}

void hash_table_rehash(HashTable* old, HashTable* new) {}

void hash_table_add(HashTable* self, void* key, void* value) {
  if ((f64)self->population >= (MAX_HASH_TABLE_LOAD * (f64)array_get(&hash_primes_array, u64, self->prime_idx))) {
    if (self->prime_idx != hash_primes_array.length) {
      self->prime_idx++;
      // need to rehash; as it is, this loses all entries except for the new one
      HashTable* new_table = allocator_alloc(self->alloc, sizeof(HashTable));
      hash_table_init(new_table, self->alloc, self->hash_func, self->comp_func, self->free_key, self->free_value);
      new_table->prime_idx = self->prime_idx;
      vector_reserve(&self->entries, array_get(&hash_primes_array, u64, self->prime_idx));
    }
  }

  u64 key_hash = self->hash_func(key);
  usize key_loc = key_hash % self->entries.size;
  HashTableEntry entry = vector_get(&self->entries, HashTableEntry, key_loc);
}

void* hash_table_remove(HashTable* self, void* key);

void* hash_table_get(HashTable* self, void* key);

void hash_table_free(HashTable* self) { allocator_free(self->alloc, self); }

void hash_table_iter_init(HashTableIter* self, HashTable* table);

void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self, HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);
