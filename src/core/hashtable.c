// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "core/hashtable.h"
#include "core/list.h"
#include "core/allocator.h"
#include "core/vector.h"

#include <stdio.h>

constexpr f64 MAX_HASH_TABLE_LOAD = 0.7;

typedef struct HashTableResult {
  List* entry_list;
  HashTableEntry* entry;
  usize entry_idx;
  usize key_loc;
} HashTableResult;

static inline void hash_table_free_key_value(HashTable* self, HashTableEntry* entry) {
  if (self->free_key) {
    self->free_key(entry->key);
  }

  if (self->free_value) {
    self->free_value(entry->value);
  }
}

static inline u64 _htpi(u64 idx) { return array_get(&hash_primes_array, u64, idx); }

static inline void hash_table_rehash(HashTable* self) {
  Vector new_vec = {};
  Vector old_vec = self->entries;
  u64 new_prime_size = _htpi(self->prime_idx);
  vector_init(&new_vec, self->alloc, sizeof(List), new_prime_size, (VectorFreeItem)list_deinit);
  vector_zero_fill(&new_vec);
  ListInitArgs list_args = {.alloc = self->alloc, .item_size = sizeof(HashTableEntry)};
  vector_item_init(&new_vec, (VectorInitItem)list_init_args, &list_args);

  self->entries = new_vec;
  self->population = 0;

  for (usize idx = 0; idx < old_vec.size; idx++) {
    List* entry_list = vector_get(&old_vec, List, idx);
    ListIter entry_list_iter = {};
    HashTableEntry* entry = NULL;
    list_iter_init(&entry_list_iter, entry_list);

    while (list_iter_next(&entry_list_iter, (void**)&entry)) {
      hash_table_insert(self, entry->key, entry->value);
      hash_table_free_key_value(self, entry);
    }
  }

  vector_deinit(&old_vec);
}

static inline HashTableResult hash_table_find_entry(HashTable* self, void* key) {
  HashTableResult result = {};
  usize key_loc = self->hash_func(key) % self->entries.size;
  List* entry_list = vector_get(&self->entries, List, key_loc);
  HashTableEntry* entry = NULL;
  ListIter entry_list_iter = list_iter(entry_list);
  usize entry_idx = 0;

  while (list_iter_next(&entry_list_iter, (void**)&entry)) {
    if (self->comp_func(entry->key, key)) {
      return (HashTableResult){.entry_list = entry_list, .entry = entry, .entry_idx = entry_idx, .key_loc = key_loc};
    }
    entry_idx++;
  }

  return result;
}

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, CopyItem copy_key,
                          CopyItem copy_value, FreeItem free_key, FreeItem free_value) {
  HashTable* table = new(alloc, sizeof(HashTable));
  hash_table_init(table, alloc, hash_func, key_comp, copy_key, copy_value, free_key, free_value);

  return table;
}

void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, CopyItem copy_key,
                     CopyItem copy_value, FreeItem free_key, FreeItem free_value) {
  usize prime_idx = 0;
  Vector entry_vec;
  vector_init(&entry_vec, alloc, sizeof(List), _htpi(prime_idx), (VectorFreeItem)list_deinit);
  vector_zero_fill(&entry_vec);
  ListInitArgs list_args = {.alloc = alloc, .item_size = sizeof(HashTableEntry)};
  vector_item_init(&entry_vec, (VectorInitItem)list_init_args, &list_args);

  *self = (HashTable){
      .alloc = alloc,
      .comp_func = key_comp,
      .hash_func = hash_func,
      .copy_key = copy_key,
      .copy_value = copy_value,
      .free_key = free_key,
      .free_value = free_value,
      .entries = entry_vec,
      .prime_idx = prime_idx,
      .population = 0,
  };
}

void hash_table_deinit(HashTable* self) {
  for (usize i = 0; i < self->entries.size; i++) {
    List* entry_list = vector_get(&self->entries, List, i);
    for (usize j = 0; j < entry_list->length; j++) {
      HashTableEntry* entry = list_get(entry_list, j);
      hash_table_free_key_value(self, entry);
    }
  }
  vector_free(&self->entries);
}

void hash_table_insert(HashTable* self, void* key, void* value) {
  if ((f64)self->population >= (MAX_HASH_TABLE_LOAD * (f64)_htpi(self->prime_idx))) {
    if (self->prime_idx != hash_primes_array.length - 1) {
      self->prime_idx++;
      hash_table_rehash(self);
    }
  }

  usize key_loc = self->hash_func(key) % self->entries.size;
  List* entry_list = vector_get(&self->entries, List, key_loc);

  void* _key = NULL;
  if (self->copy_key) {
    _key = self->copy_key(key, self->alloc);
  } else {
    _key = key;
  }

  void* _value = NULL;
  if (self->copy_value) {
    _value = self->copy_value(value, self->alloc);
  } else {
    _value = value;
  }

  HashTableEntry entry = {.key = _key, .value = _value};
  list_push(entry_list, &entry);

  self->population++;
}

void hash_table_remove(HashTable* self, void* key) {
  HashTableResult result = hash_table_find_entry(self, key);
  List* entry_list = result.entry_list;

  if (entry_list) {
    hash_table_free_key_value(self, result.entry);
    list_remove(entry_list, result.entry_idx);
    self->population--;
  }
}

void* hash_table_get(HashTable* self, void* key) {
  HashTableResult result = hash_table_find_entry(self, key);
  if (result.entry) {
    return result.entry->value;
  }

  return NULL;
}

void hash_table_free(HashTable* self) {
  for (usize i = 0; i < self->entries.size; i++) {
    List* entry_list = vector_get(&self->entries, List, i);
    for (usize j = 0; j < entry_list->length; j++) {
      HashTableEntry* entry = list_get(entry_list, j);
      hash_table_free_key_value(self, entry);
    }
  }
  vector_free(&self->entries);
  allocator_free(self->alloc, self);
}

static inline void _hash_table_iter_init(HashTableIter* self, HashTable* table, usize vector_idx) {
  ListIter list_iter = {};
  list_iter_init(&list_iter, vector_get(&table->entries, List, vector_idx));
  self->entry_list_iter = list_iter;
  self->table = table;
  self->idx = vector_idx;
  
}

void hash_table_iter_init(HashTableIter* self, HashTable* table) {
  _hash_table_iter_init(self, table, 0);
}

bool hash_table_iter_next(HashTableIter* self, void** key, void** value) {
  HashTableEntry* entry = NULL;
  while (self->idx < self->table->entries.size) {
    if (list_iter_next(&self->entry_list_iter, (void**)&entry)) {
      *key = entry->key;
      *value = entry->value;
      return true;
    } else {
      self->idx++;
      if (self->idx < self->table->entries.size) {
        _hash_table_iter_init(self, self->table, self->idx);
      }
    }
  }

  return false;
}

bool hash_table_key_iter_next(HashTableIter* self, void** key) {
  void* value = NULL;
  return hash_table_iter_next(self, key, &value);
}

bool hash_table_value_iter_next(HashTableIter* self, void** value) {
  void* key = NULL;
  return hash_table_iter_next(self, &key, value);
}
