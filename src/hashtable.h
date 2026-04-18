// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#pragma once

#include "allocator.h"
#include "modint.h"
#include "vector.h"

typedef u64 (*HashFunc)(const void* value);

typedef u64 (*KeyEqualFunc)(const void* lhs, const void* rhs);

typedef void (*HashTableFreeEntry)(void* key, void* value);

typedef void (*HashTableFreeKey)(void* key);

typedef void (*HashTableFreeValue)(void* value);

typedef struct {
  Allocator* alloc;
  u64 prime;
  usize population;
  Vector entries;
  HashFunc hash_func;
  KeyEqualFunc comp_func;
  HashTableFreeKey free_key;
  HashTableFreeValue free_value;
} HashTable;

typedef struct HashTableEntry HashTableEntry;

struct HashTableEntry {
  void* key;
  void* value;
  HashTableEntry* last_entry;
  HashTableEntry* next_entry;
};

typedef struct {
  HashTable* table;
  usize idx;
} HashTableIter;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func,
                          KeyEqualFunc key_comp, HashTableFreeKey free_key,
                          HashTableFreeValue free_value);

void hash_table_init(HashTable* table, Allocator* alloc, HashFunc hash_func,
                     KeyEqualFunc key_comp, HashTableFreeKey free_key,
                     HashTableFreeValue free_value);

void hash_table_add(HashTable* self, void* key, void* value);

void* hash_table_remove(HashTable* self, void* key);

void* hash_table_get(HashTable* self, void* key);

void hash_table_free(HashTable* self);

void hash_table_iter_init(HashTableIter* self, HashTable* table);

void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self,
                                HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);

static u64 hash_primes[] = {
    53,        97,        193,       389,       769,        1543,     3079,
    6151,      12289,     24593,     49157,     98317,      196613,   393241,
    786433,    1572869,   3145739,   6291469,   12582917,   25165843, 50331653,
    100663319, 201326611, 402653189, 805306457, 1610612741,
};
