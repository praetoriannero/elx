// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "hashtable.h"
#include "core/allocator.h"
#include "core/vector.h"

/*
  Maximum load of the hash table before it rehashes
*/
constexpr f64 MAX_HASH_TABLE_LOAD = 0.7;

/*
  Create a new, heap-allocated HashTable
*/
HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                          FreeValueFunc free_value) {
  HashTable* table = NULL;
  hash_table_init(table, alloc, hash_func, key_comp, free_key, free_value);

  return table;
}


/*
  Convenience method for getting the hash_primes_array index
*/
static inline u64 _htpi(u64 idx) {
  return array_get(&hash_primes_array, u64, idx);
}


/*
  Initialize a HashTable
*/
void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                     FreeValueFunc free_value) {
  usize prime_idx = hash_primes[0];
  Vector entry_vec;
  vector_init(&entry_vec, alloc, sizeof(usize), _htpi(self->prime_idx), NULL);
  vector_zero_fill(&entry_vec);

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

/*
  Rehash the hash table
*/
void hash_table_rehash(HashTable* self) {
  Vector new_vec = {};
  Vector old_vec = self->entries;
  self->population = 0;

  u64 new_prime_size = _htpi(self->prime_idx);
  vector_init(&new_vec, self->alloc, sizeof(HashTableEntry), new_prime_size, NULL);
  vector_zero_fill(&new_vec);

  for (usize idx = 0; idx < self->entries.size; idx++) {
    HashTableEntry entry = vector_get(&self->entries, HashTableEntry, idx);
    HashTableEntry* child = entry.next_entry;
    while (child) {
      u64 hash = self->hash_func(child->key);
      usize new_loc = hash % new_prime_size;
      // hash_table_insert
    }
  }
}

/*
  Insert into the hash table
*/
void hash_table_insert(HashTable* self, void* key, void* value) {
  if ((f64)self->population >= (MAX_HASH_TABLE_LOAD * (f64)_htpi(self->prime_idx))) {
    if (self->prime_idx != hash_primes_array.length - 1) {
      self->prime_idx++;
      hash_table_rehash(self);
    }
  }

  u64 key_hash = self->hash_func(key);
  usize key_loc = key_hash % self->entries.size;
  HashTableEntry entry = vector_get(&self->entries, HashTableEntry, key_loc);
  if (!entry.initialized) {
    entry.initialized = true;
    entry.key = key;
    entry.value = value;
    vector_insert(&self->entries, key_loc, &entry);
  } else {
    HashTableEntry* new_entry = allocator_alloc(self->alloc, sizeof(HashTableEntry));
    new_entry->initialized = true;
    new_entry->key = key;
    new_entry->value = value;
    new_entry->last_entry = &entry;
  }

  self->population++;
}

/*
  Remove an entry from the hash table and return it, if it exists, else return null
*/
void* hash_table_remove(HashTable* self, void* key);

/*
  Get an entry from the hash table by its key, if it exists, else return null
*/
void* hash_table_get(HashTable* self, void* key);

/*
  Free the hash table from the heap
*/
void hash_table_free(HashTable* self) { allocator_free(self->alloc, self); }

/*
  Initialize a HashTableIter object
*/
void hash_table_iter_init(HashTableIter* self, HashTable* table);

/*
  Iterate through a hash table using its iterator
*/
void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self, HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);
