// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "hashtable.h"
#include "allocator.h"

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func,
                         KeyEqualFunc key_comp, HashTableFreeEntry free_entry) {
  HashTable* table = NULL;
  hash_table_init(table, alloc, hash_func, key_comp, free_entry);

  return table;
}

void hash_table_init(HashTable* table, Allocator* alloc, HashFunc hash_func,
                    KeyEqualFunc key_comp, HashTableFreeEntry free_entry) {
  u64 first_prime = hash_primes[0];
  Vector entry_vec;
  vector_init(alloc, &entry_vec, sizeof(usize), first_prime);

  *table = (HashTable){
      .alloc = alloc,
      .comp_func = key_comp,
      .hash_func = hash_func,
      .entries = entry_vec,
      .prime = first_prime,
      .free_entry = free_entry,
      .population = 0,
  };
}

void* hash_table_add(HashTable* table, void* key, void* value);

void* hash_table_remove(HashTable* table, void* key);

void* hash_table_get(HashTable* table, void* key);

void hash_table_free(HashTable* table) {
  vector_free(&table->entries, (VectorFreeInner)table->free_entry);
  allocator_free(table->alloc, table);
}

void hash_table_iter_init(HashTableIter* table_iter, HashTable* table);

void hash_table_iter_next(HashTableIter* table_iter, void** key, void** value);

void hash_table_key_iter_init(HashTableKeyIter* table_iter, HashTable* table);

void hash_table_key_iter_next(HashTableKeyIter* table_iter, void** key);

void hash_table_value_iter_init(HashTableValueIter* table_iter,
                               HashTable* table);

void hash_table_value_iter_next(HashTableValueIter* table_iter, void** value);
