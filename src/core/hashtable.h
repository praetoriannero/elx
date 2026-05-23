#pragma once

#include "core/allocator.h"
#include "core/array.h"
#include "core/list.h"
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

/*  @brief Function type for freeing keys and values
 *  @param item: the item to free
 */
typedef void (*FreeItem)(void* item);

/*  @brief Function type for copying keys and values
 *  @param item: the item to free
 *  @param alloc: the allocator to use for the copy
 */
typedef void* (*CopyItem)(const void* item, Allocator* alloc);

/*  @brief Struct for managing the state of the hash table
 */
typedef struct {
    Allocator* alloc;
    usize prime_idx;
    usize population;
    Vector entries;
    HashFunc hash_func;
    KeyEqualFunc comp_func;
    FreeItem free_key;
    FreeItem free_value;
    CopyItem copy_key;
    CopyItem copy_value;
} HashTable;

typedef struct HashTableEntry HashTableEntry;

struct HashTableEntry {
    void* key;
    void* value;
};

typedef struct {
    HashTable* table;
    usize idx;
    ListIter entry_list_iter;
} HashTableIter;

HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, CopyItem copy_key,
                          CopyItem copy_value, FreeItem free_key, FreeItem free_value);

void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, CopyItem copy_key,
                     CopyItem copy_value, FreeItem free_key, FreeItem free_value);

void hash_table_deinit(HashTable* self);

void hash_table_insert(HashTable* self, void* key, void* value);

void hash_table_remove(HashTable* self, void* key);

void* hash_table_get(HashTable* self, void* key);

void hash_table_free(HashTable* self);

void hash_table_iter_init(HashTableIter* self, HashTable* table);

bool hash_table_iter_next(HashTableIter* self, void** key, void** value);

bool hash_table_key_iter_next(HashTableIter* self, void** key);

bool hash_table_value_iter_next(HashTableIter* self, void** value);

static u64 hash_primes[] = {
    53,       97,       193,      389,       769,       1543,      3079,      6151,       12289,
    24593,    49157,    98317,    196613,    393241,    786433,    1572869,   3145739,    6291469,
    12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741,
};

static Array hash_primes_array = array_from_ptr(hash_primes);
