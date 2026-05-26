/**
 * @file modsize.h
 * @brief Pointer-sized integer type aliases.
 *
 * Provides usize and isize, which are guaranteed to be the same width
 * as pointers on the target platform. Used for indexing, sizes, and
 * memory-related quantities throughout the compiler.
 */

#pragma once

#include <stdint.h>

/** Unsigned pointer-sized integer, used for sizes and indices. */
typedef uintptr_t usize;
/** Signed pointer-sized integer, used for offsets. */
typedef intptr_t isize;
