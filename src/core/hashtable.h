/**
 * @file hashtable.h
 * @brief Generic hash table implementation.
 *
 * Provides a type-erased, open-addressing hash table with configurable
 * hashing, equality, copy, and free callbacks. Supports insertion,
 * lookup, removal, and iteration over keys, values, or entries.
 */

#pragma once

#include "core/allocator.h"
#include "core/array.h"
#include "core/list.h"
#include "core/vector.h"

/**
 * @typedef HashFunc
 * @brief Function pointer type for hashing keys.
 *
 * @param value  The key value to hash.
 * @return       The resulting hash as a 64-bit integer.
 */
typedef u64 (*HashFunc)(const void* value);

/**
 * @typedef KeyEqualFunc
 * @brief Function pointer type for comparing two keys for equivalency.
 *
 * @param lhs  The first key.
 * @param rhs  The second key to compare against.
 * @return      true if the keys are equivalent, false otherwise.
 */
typedef bool (*KeyEqualFunc)(const void* lhs, const void* rhs);

/**
 * @typedef FreeItem
 * @brief Function pointer type for freeing keys and values.
 *
 * @param item  The item to free.
 */
typedef void (*FreeItem)(void* item);

/**
 * @typedef CopyItem
 * @brief Function pointer type for copying keys and values.
 *
 * @param item  The item to copy.
 * @param alloc  Allocator to use for the copy.
 * @return       Pointer to the newly allocated copy, or NULL on failure.
 */
typedef void* (*CopyItem)(const void* item, Allocator* alloc);

/**
 * @struct HashTable
 * @brief State and configuration for a hash table.
 *
 * Manages a collection of key-value pairs with configurable hashing and
 * equality functions. Entries are stored in a Vector and grow dynamically
 * as needed.
 */
typedef struct {
  /** Allocator used for all internal allocations. */
  Allocator* alloc;
  /** Index into the prime table for the current bucket count. */
  usize prime_idx;
  /** Number of entries currently stored. */
  usize population;
  /** Vector of hash table entries. */
  Vector entries;
  /** Function to compute the hash of a key. */
  HashFunc hash_func;
  /** Function to compare two keys for equivalency. */
  KeyEqualFunc comp_func;
  /** Function to free a key. */
  FreeItem free_key;
  /** Function to free a value. */
  FreeItem free_value;
  /** Function to copy a key. */
  CopyItem copy_key;
  /** Function to copy a value. */
  CopyItem copy_value;
} HashTable;

/**
 * @struct HashTableEntry
 * @brief A single key-value pair in the hash table.
 */
typedef struct HashTableEntry HashTableEntry;

/** A single key-value pair in the hash table. */
struct HashTableEntry {
  /** The key. */
  void* key;
  /** The value. */
  void* value;
};

/**
 * @struct HashTableIter
 * @brief Iterator over a hash table.
 *
 * Tracks the current position during sequential traversal of entries,
 * keys, or values.
 */
typedef struct {
  /** Pointer to the table being iterated. */
  HashTable* table;
  /** Current index into the table's entry list. */
  usize idx;
  /** Underlying list iterator for entry traversal. */
  ListIter entry_list_iter;
} HashTableIter;

/**
 * @brief Allocate and initialize a new hash table.
 *
 * @param alloc       Allocator for internal allocations.
 * @param hash_func   Function to compute key hashes.
 * @param key_comp    Function to compare keys for equivalency.
 * @param copy_key    Function to copy keys, or NULL if keys are not owned.
 * @param copy_value  Function to copy values, or NULL if values are not owned.
 * @param free_key    Function to free keys, or NULL if keys are not owned.
 * @param free_value  Function to free values, or NULL if values are not owned.
 * @return            A newly allocated HashTable on success, or NULL on failure.
 */
HashTable* hash_table_new(Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, CopyItem copy_key,
                          CopyItem copy_value, FreeItem free_key, FreeItem free_value);

/**
 * @brief Initialize an existing hash table with the given parameters.
 *
 * @param alloc       Allocator for internal allocations.
 * @param hash_func   Function to compute key hashes.
 * @param key_comp    Function to compare keys for equivalency.
 * @param copy_key    Function to copy keys, or NULL if keys are not owned.
 * @param copy_value  Function to copy values, or NULL if values are not owned.
 * @param free_key    Function to free keys, or NULL if keys are not owned.
 * @param free_value  Function to free values, or NULL if values are not owned.
 */
void hash_table_init(HashTable* self, Allocator* alloc, HashFunc hash_func, KeyEqualFunc key_comp, CopyItem copy_key,
                     CopyItem copy_value, FreeItem free_key, FreeItem free_value);

/**
 * @brief Deinitialize and free all resources owned by the hash table.
 *
 * Frees all entries, keys, and values according to the registered free
 * callbacks. The HashTable struct itself is not freed.
 *
 * @param self  Pointer to the HashTable to deinitialize.
 */
void hash_table_deinit(HashTable* self);

/**
 * @brief Insert a key-value pair into the hash table.
 *
 * If the key already exists, its value is replaced. The key and value
 * are copied if copy callbacks are registered.
 *
 * @param self   Pointer to the HashTable.
 * @param key    The key to insert.
 * @param value  The value to associate with the key.
 */
void hash_table_insert(HashTable* self, void* key, void* value);

/**
 * @brief Remove a key-value pair from the hash table by key.
 *
 * The key and value are freed using the registered free callbacks if
 * they are non-NULL.
 *
 * @param self  Pointer to the HashTable.
 * @param key   The key to remove.
 */
void hash_table_remove(HashTable* self, void* key);

/**
 * @brief Look up a value by key in the hash table.
 *
 * @param self  Pointer to the HashTable.
 * @param key   The key to look up.
 * @return      Pointer to the associated value, or NULL if the key is not found.
 */
void* hash_table_get(HashTable* self, void* key);

/**
 * @brief Free the HashTable struct itself.
 *
 * Does not free the HashTable's internal resources — use hash_table_deinit()
 * for that.
 *
 * @param self  Pointer to the HashTable to free.
 */
void hash_table_free(HashTable* self);

/**
 * @brief Initialize a hash table iterator.
 *
 * @param self   Pointer to the iterator to initialize.
 * @param table  Pointer to the table to iterate over.
 */
void hash_table_iter_init(HashTableIter* self, HashTable* table);

/**
 * @brief Advance the iterator to the next entry and retrieve its key and value.
 *
 * @param self    Pointer to the iterator.
 * @param key     Output pointer for the key, or NULL.
 * @param value   Output pointer for the value, or NULL.
 * @return         true if an entry was retrieved, false if iteration is complete.
 */
bool hash_table_iter_next(HashTableIter* self, void** key, void** value);

/**
 * @brief Advance the iterator to the next entry and retrieve its key.
 *
 * @param self  Pointer to the iterator.
 * @param key   Output pointer for the key.
 * @return       true if an entry was retrieved, false if iteration is complete.
 */
bool hash_table_key_iter_next(HashTableIter* self, void** key);

/**
 * @brief Advance the iterator to the next entry and retrieve its value.
 *
 * @param self   Pointer to the iterator.
 * @param value  Output pointer for the value.
 * @return        true if an entry was retrieved, false if iteration is complete.
 */
bool hash_table_value_iter_next(HashTableIter* self, void** value);

/**
 * @brief Sequence of prime numbers used for dynamic bucket count resizing.
 *
 * The hash table uses these primes to determine the number of buckets,
 * growing the table by advancing to the next prime when the load factor
 * exceeds a threshold.
 */
static u64 hash_primes[] = {
    53,       97,       193,      389,       769,       1543,      3079,      6151,       12289,
    24593,    49157,    98317,    196613,    393241,    786433,    1572869,   3145739,    6291469,
    12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741,
};

/**
 * @brief Array view of the hash_primes lookup table.
 */
static Array hash_primes_array = array_from_ptr(hash_primes);
