// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include <stdio.h>
#include "hashtable.h"
#include "core/allocator.h"
#include "core/vector.h"

constexpr f64 MAX_HASH_TABLE_LOAD = 0.7;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                          FreeValueFunc free_value) {
  HashTable* table = NULL;
  hash_table_init(table, alloc, hash_func, key_comp, free_key, free_value);

  return table;
}

static inline void printd(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  printf("[DEBUG] ");
  vprintf(fmt, args);
  printf("\n");
  fflush(stdout);

  va_end(args);
}

static inline u64 _htpi(u64 idx) { return array_get(&hash_primes_array, u64, idx); }

void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                     FreeValueFunc free_value) {
  usize prime_idx = 0;
  Vector entry_vec;
  vector_init(&entry_vec, alloc, sizeof(HashTableEntry), _htpi(self->prime_idx), NULL);
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

static inline void hash_table_rehash(HashTable* self) {
  Vector new_vec = {};
  Vector old_vec = self->entries;
  u64 new_prime_size = _htpi(self->prime_idx);
  vector_init(&new_vec, self->alloc, sizeof(HashTableEntry), new_prime_size, NULL);
  vector_zero_fill(&new_vec);

  self->entries = new_vec;
  self->population = 0;

  for (usize idx = 0; idx < old_vec.size; idx++) {
    HashTableEntry* entry = vector_get(&old_vec, HashTableEntry, idx);
    if (!entry->initialized) {
      continue;
    }

    HashTableEntry* child = entry->child_entry;
    while (child != NULL && child->initialized) {
      hash_table_insert(self, child->key, child->value);
      child = child->child_entry;
      if (child != NULL && child->initialized) {
        HashTableEntry* parent = child->parent_entry;
        allocator_free(self->alloc, child->parent_entry);
      }
    }

    hash_table_insert(self, entry->key, entry->value);
  }

  vector_deinit(&old_vec);
}

void hash_table_insert(HashTable* self, void* key, void* value) {
  if ((f64)self->population >= (MAX_HASH_TABLE_LOAD * (f64)_htpi(self->prime_idx))) {
    if (self->prime_idx != hash_primes_array.length - 1) {
      self->prime_idx++;
      hash_table_rehash(self);
    }
  }

  u64 key_hash = self->hash_func(key);
  usize key_loc = key_hash % self->entries.size;
  HashTableEntry* entry = vector_get(&self->entries, HashTableEntry, key_loc);
  if (!entry->initialized) {
    entry->initialized = true;
    entry->key = key;
    entry->value = value;
    vector_insert(&self->entries, key_loc, entry);
  } else {
    while (entry->child_entry != NULL) {
      entry = entry->child_entry;
    }

    HashTableEntry* new_entry = allocator_alloc(self->alloc, sizeof(HashTableEntry));
    new_entry->initialized = true;
    new_entry->key = key;
    new_entry->value = value;
    new_entry->parent_entry = entry;
    new_entry->child_entry = NULL;
    entry->child_entry = new_entry;
  }

  self->population++;
}

typedef struct HashTableResult {
  HashTableEntry* entry;
  usize key_loc;
} HashTableResult;

static inline HashTableResult hash_table_find_entry(HashTable* self, void* key) {
  HashTableResult result = {};
  u64 key_hash = self->hash_func(key);
  usize key_loc = key_hash % self->entries.size;
  HashTableEntry* entry = vector_get(&self->entries, HashTableEntry, key_loc);

  if (!entry->initialized) {
    return result;
  }

  while (entry) {
    if (self->comp_func(key, entry->key)) {
      result.entry = entry;
      result.key_loc = key_loc;
      return result;
    }

    entry = entry->child_entry;
  }

  return result;
}

void hash_table_remove(HashTable* self, void* key) {
  HashTableResult result = hash_table_find_entry(self, key);
  HashTableEntry* entry = result.entry;

  if (entry) {
    entry->initialized = false;
    HashTableEntry* parent = entry->parent_entry;
    HashTableEntry* child = entry->child_entry;
    if (parent) {
      parent->child_entry = child;
    }

    if (child) {
      child->parent_entry = parent;
    }

    if (!parent && child) {
      vector_insert(&self->entries, result.key_loc, child);
    }

    if (parent) {
      allocator_free(self->alloc, entry);
    }

    self->population--;
  }
}

void* hash_table_get(HashTable* self, void* key) {
  HashTableResult result = hash_table_find_entry(self, key);
  if (result.entry && result.entry->initialized) {
    return result.entry->value;
  }

  return NULL;
}

void hash_table_free(HashTable* self) {
  vector_free(&self->entries);
  allocator_free(self->alloc, self);
}

void hash_table_iter_init(HashTableIter* self, HashTable* table);

void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self, HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);
