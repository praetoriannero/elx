// https://benhoyt.com/writings/hash-table-in-c/
// refer to the above
#pragma once

#include "core/allocator.h"
#include "core/array.h"
#include "core/vector.h"

/*  @brief Type for hashing keys
 *  @param value the value to hash
 */
typedef u64 (*HashFunc)(const void* value);

/*  @brief Type for comparing two keys for equivalency
 *  @param lhs a key to compare
 *  @param rhs the other key to compare against lhs
 */
typedef bool (*KeyEqualFunc)(const void* lhs, const void* rhs);

typedef void (*FreeKeyFunc)(void* key);

typedef void (*FreeValueFunc)(void* value);

typedef struct {
  Allocator* alloc;
  usize prime_idx;
  usize population;
  Vector entries;
  HashFunc hash_func;
  KeyEqualFunc comp_func;
  FreeKeyFunc free_key;
  FreeValueFunc free_value;
} HashTable;

typedef struct HashTableEntry HashTableEntry;

struct HashTableEntry {
  void* key;
  void* value;
  bool initialized;
  HashTableEntry* last_entry;
  HashTableEntry* next_entry;
};

typedef struct {
  HashTable* table;
  usize idx;
} HashTableIter;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, FreeKeyFunc free_key,
                          FreeValueFunc free_value);

void hash_table_init(HashTable* table, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp,
                     FreeKeyFunc free_key, FreeValueFunc free_value);

void hash_table_insert(HashTable* self, void* key, void* value);

void* hash_table_remove(HashTable* self, void* key);

void* hash_table_get(HashTable* self, void* key);

void hash_table_free(HashTable* self);

void hash_table_iter_init(HashTableIter* self, HashTable* table);

void hash_table_iter_next(HashTableIter* self, void** key, void** value);

void hash_table_key_iter_init(HashTableIter* self, HashTable* table);

void hash_table_key_iter_next(HashTableIter* self, void** key);

void hash_table_value_iter_init(HashTableIter* self, HashTable* table);

void hash_table_value_iter_next(HashTableIter* self, void** value);

static u64 hash_primes[] = {
    53,       97,       193,      389,       769,       1543,      3079,      6151,       12289,
    24593,    49157,    98317,    196613,    393241,    786433,    1572869,   3145739,    6291469,
    12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741,
};

// Array hash_primes_array =
//     (Array){.data = hash_primes, .item_size = sizeof(hash_primes[0]), .length = sizeof(hash_primes)};

Array hash_primes_array = array_from_ptr(hash_primes);
