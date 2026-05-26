/**
 * @file hash.h
 * @brief Hashing utilities.
 *
 * Provides FNV-1a hash computation and string hashing functions used
 * throughout the compiler for symbol tables and hash-based collections.
 */

#pragma once

#include "core/modprim.h"

/**
 * @brief Compute the FNV-1a hash of a byte buffer.
 *
 * @param data         Pointer to the input data.
 * @param data_length  Number of bytes to hash.
 * @return             The FNV-1a hash value.
 */
u64 fnv1a(u8* data, usize data_length);

/**
 * @brief Compute the hash of a null-terminated string.
 *
 * @param ptr  Null-terminated string to hash.
 * @return     The hash value of the string.
 */
u64 hash_str(const char* ptr);
