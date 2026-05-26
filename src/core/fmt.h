/**
 * @file fmt.h
 * @brief Formatted string builder.
 *
 * Provides a simple printf-style formatting function that writes the
 * result into an allocator-managed buffer.
 */

#pragma once

#include "core/allocator.h"

/**
 * @brief Format a string and return it as an allocator-allocated buffer.
 *
 * Behaves like printf, formatting the given arguments according to the
 * format string. The resulting string is heap-allocated via the provided
 * allocator and must be freed by the caller.
 *
 * @param allocator  Allocator to use for the result buffer.
 * @param fmt        printf-style format string.
 * @return           Formatted string on success, or NULL on failure.
 */
char* fmt(Allocator* allocator, const char* fmt, ...);
