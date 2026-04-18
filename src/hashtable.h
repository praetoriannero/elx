// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#pragma once

#include "allocator.h"
#include "modint.h"
#include "vector.h"

typedef u64 (*HashFunc)(const void* value);

typedef u64 (*KeyEqualFunc)(const void* lhs, const void* rhs);

typedef void (*HashTableFreeEntry)(void* key, void* value);

typedef struct {
  Vector entries;
  HashFunc hash_func;
  KeyEqualFunc comp_func;
  Allocator* alloc;
  HashTableFreeEntry free_entry;
  u64 prime;
  usize population;
} HashTable;

typedef struct {
  void* key;
  void* value;
} HashTableEntry;

typedef struct {
  HashTable* table;
  usize idx;
} HashTableIter;

typedef struct {
  HashTable* table;
  usize idx;
} HashTableKeyIter;

typedef struct {
  HashTable* table;
  usize idx;
} HashTableValueIter;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func,
                         KeyEqualFunc key_comp, HashTableFreeEntry free_entry);

void hash_table_init(HashTable* table, Allocator* alloc, HashFunc hash_func,
                    KeyEqualFunc key_comp, HashTableFreeEntry free_entry);

void* hash_table_add(HashTable* table, void* key, void* value);

void* hash_table_remove(HashTable* table, void* key);

void* hash_table_get(HashTable* table, void* key);

void hash_table_free(HashTable* table);

void hash_table_iter_init(HashTableIter* table_iter, HashTable* table);

void hash_table_iter_next(HashTableIter* table_iter, void** key, void** value);

void hash_table_key_iter_init(HashTableKeyIter* table_iter, HashTable* table);

void hash_table_key_iter_next(HashTableKeyIter* table_iter, void** key);

void hash_table_value_iter_init(HashTableValueIter* table_iter,
                               HashTable* table);

void hash_table_value_iter_next(HashTableValueIter* table_iter, void** value);

static u64 hash_primes[] = {
    53,        97,        193,       389,       769,        1543,     3079,
    6151,      12289,     24593,     49157,     98317,      196613,   393241,
    786433,    1572869,   3145739,   6291469,   12582917,   25165843, 50331653,
    100663319, 201326611, 402653189, 805306457, 1610612741,
};
