// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#pragma once

#include "allocator.h"
#include "modint.h"
#include "vector.h"

typedef u64 (*HashFunc)(const void* value);

typedef u64 (*KeyEqualFunc)(const void* lhs, const void* rhs);

typedef struct {
  Vector keys;
  Vector values;
  HashFunc hash_func;
  KeyEqualFunc comp_func;
} HashTable;

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

HashTable* hashtable_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc hash_comp);

void hashtable_init(HashTable* table, Allocator* alloc);

void* hashtable_add(HashTable* table, void* key, void* value);

void* hashtable_remove(HashTable* table, void* key);

void* hashtable_get(HashTable* table, void* key);

void hashtable_iter_init(HashTableIter* table_iter, HashTable* table);

void hashtable_iter_next(HashTableIter* table_iter, void** key, void** value);

void hashtable_key_iter_init(HashTableKeyIter* table_iter, HashTable* table);

void hashtable_key_iter_next(HashTableKeyIter* table_iter, void** key);

void hashtable_value_iter_init(HashTableValueIter* table_iter, HashTable* table);

void hashtable_value_iter_next(HashTableValueIter* table_iter, void** value);

