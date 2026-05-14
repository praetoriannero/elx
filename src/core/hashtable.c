// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#include "core/hashtable.h"
#include "core/list.h"
#include "core/allocator.h"
#include "core/vector.h"

constexpr f64 MAX_HASH_TABLE_LOAD = 0.7;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, usize key_size, usize value_size,
                          FreeKeyFunc free_key, FreeValueFunc free_value) {
  HashTable* table = NULL;
  hash_table_init(table, alloc, hash_func, key_comp, key_size, value_size, free_key, free_value);

  return table;
}

static inline u64 _htpi(u64 idx) { return array_get(&hash_primes_array, u64, idx); }

void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, usize key_size,
                     usize value_size, FreeKeyFunc free_key, FreeValueFunc free_value) {
  usize prime_idx = 0;
  Vector entry_vec;
  vector_init(&entry_vec, alloc, sizeof(List), _htpi(prime_idx), (VectorFreeItem)list_deinit);
  vector_zero_fill(&entry_vec);
  ListInitArgs list_args = {.alloc = alloc, .item_size = sizeof(HashTableEntry)};
  vector_item_init(&entry_vec, (VectorItemInit)list_init_args, (void*)&list_args);

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
  vector_init(&new_vec, self->alloc, sizeof(List), new_prime_size, (VectorFreeItem)list_deinit);
  vector_zero_fill(&new_vec);
  ListInitArgs list_args = {.alloc = self->alloc, .item_size = sizeof(HashTableEntry)};
  vector_item_init(&new_vec, (VectorItemInit)list_init_args, (void*)&list_args);

  self->entries = new_vec;
  self->population = 0;

  for (usize idx = 0; idx < old_vec.size; idx++) {
    List* entry_list = vector_get(&old_vec, List, idx);
    ListIter entry_list_iter = {};
    HashTableEntry* entry = NULL;
    list_iter_init(&entry_list_iter, entry_list);

    while (list_iter_next(&entry_list_iter, (void**)&entry)) {
      hash_table_insert(self, entry->key, entry->value);
    }
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

  usize key_loc = self->hash_func(key) % self->entries.size;
  List* entry_list = vector_get(&self->entries, List, key_loc);

  if (!entry_list) {
    vector_insert(&self->entries, key_loc, allocator_alloc(self->alloc, sizeof(List)));
  }

  auto entry = (HashTableEntry){.key = key, .value = value};
  list_push(entry_list, &entry);

  self->population++;
}

typedef struct HashTableResult {
  List* entry_list;
  HashTableEntry* entry;
  usize entry_idx;
  usize key_loc;
} HashTableResult;

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

void hash_table_remove(HashTable* self, void* key) {
  HashTableResult result = hash_table_find_entry(self, key);
  List* entry_list = result.entry_list;

  if (entry_list) {
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
  vector_free(&self->entries);
  allocator_free(self->alloc, self);
}

void hash_table_iter_init(HashTableIter* self, HashTable* table);

void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self, HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);
