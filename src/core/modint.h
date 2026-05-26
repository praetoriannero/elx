/**
 * @file modint.h
 * @brief Signed and unsigned integer type aliases.
 *
 * Provides fixed-width integer types (i8, u8, i16, u16, i32, u32, i64, u64)
 * used consistently throughout the compiler for numeric operations.
 */

#pragma once

#include <stdint.h>

/** Signed 8-bit integer. */
typedef int8_t i8;
/** Unsigned 8-bit integer. */
typedef uint8_t u8;
/** Signed 16-bit integer. */
typedef int16_t i16;
/** Unsigned 16-bit integer. */
typedef uint16_t u16;
/** Signed 32-bit integer. */
typedef int32_t i32;
/** Unsigned 32-bit integer. */
typedef uint32_t u32;
/** Signed 64-bit integer. */
typedef int64_t i64;
/** Unsigned 64-bit integer. */
typedef uint64_t u64;
