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

void hash_table_init(HashTable* table, Allocator* alloc, HashFunc hash_func,
                     KeyEqualFunc key_comp, HashTableFreeKey free_key,
                     HashTableFreeValue free_value) {
  u64 first_prime = hash_primes[0];
  Vector entry_vec;
  vector_init(alloc, &entry_vec, sizeof(usize), first_prime);

  *table = (HashTable){
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

void hash_table_add(HashTable* table, void* key, void* value) {
  u64 key_hash = table->hash_func(key);
  usize key_loc = key_hash % table->entries.size;
}

void* hash_table_remove(HashTable* table, void* key);

void* hash_table_get(HashTable* table, void* key);

void hash_table_free(HashTable* table) { allocator_free(table->alloc, table); }

void hash_table_iter_init(HashTableIter* table_iter, HashTable* table);

void hash_table_iter_next(HashTableIter* table_iter, void** key, void** value);

void hash_table_key_iter_init(HashTableKeyIter* table_iter, HashTable* table);

void hash_table_key_iter_next(HashTableKeyIter* table_iter, void** key);

void hash_table_value_iter_init(HashTableValueIter* table_iter,
                                HashTable* table);

void hash_table_value_iter_next(HashTableValueIter* table_iter, void** value);
