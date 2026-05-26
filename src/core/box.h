/**
 * @file box.h
 * @brief Opaque pointer wrapper for arbitrary data.
 *
 * Box provides a thin abstraction over a void pointer paired with an
 * allocator, enabling safe ownership transfer and automatic cleanup of
 * heap-allocated data across the compiler's core modules.
 */

#pragma once

#include "core/allocator.h"

/**
 * @struct Box
 * @brief Opaque data container with allocator-backed lifecycle.
 *
 * A Box holds a single opaque pointer and the allocator used to manage
 * its lifetime. It is intended for wrapping heap-allocated values that
 * need to be passed around, copied, or freed through a common interface.
 */
typedef struct {
  /** Pointer to the wrapped data. */
  void** ptr;
  /** Allocator used to manage the box's memory. */
  Allocator* alloc;
} Box;

/**
 * @brief Create a new Box wrapping the given pointer.
 *
 * @param alloc  Allocator to use for internal allocations.
 * @param ptr    Pointer to wrap. May be NULL.
 * @return       A newly allocated Box on success, or NULL on failure.
 */
Box box_make(Allocator* alloc, void* ptr);

/**
 * @brief Initialize an existing Box with a pointer and allocator.
 *
 * Use this function when the Box storage is already allocated (e.g.,
 * stack-allocated) and needs to be set up before use.
 *
 * @param self   Pointer to the Box to initialize.
 * @param alloc  Allocator to associate with this Box.
 * @param ptr    Pointer to wrap. May be NULL.
 */
void box_init(Box* self, Allocator* alloc, void* ptr);

/**
 * @brief Deinitialize and free resources owned by the Box.
 *
 * Releases the wrapped pointer via the associated allocator and
 * clears the Box's internal state. Safe to call on an uninitialized Box.
 *
 * @param self  Pointer to the Box to deinitialize.
 */
void box_deinit(Box* self);

/**
 * @brief Create a deep copy of the Box.
 *
 * Allocates a new Box and duplicates the wrapped pointer into a newly
 * allocated region. The caller is responsible for freeing the returned
 * Box when done.
 *
 * @param self   Pointer to the Box to copy.
 * @param alloc  Allocator to use for the new Box and its data.
 * @return       A newly allocated Box on success, or NULL on failure.
 */
Box* box_copy(Box* self, Allocator* alloc);

/**
 * @brief Free the Box itself (but not the wrapped pointer).
 *
 * This function frees the Box struct allocated by box_copy(). It does
 * not free the wrapped pointer — use box_deinit() for that.
 *
 * @param self  Pointer to the Box to free.
 */
void box_free(Box* self);

/**
 * @brief Unwrap the opaque pointer from the Box.
 *
 * @param self  Pointer to the Box.
 * @return      The wrapped void pointer, or NULL if unwrapping failed.
 */
void* box_unwrap(Box* self);

/**
 * @brief Unwrap and cast the Box's pointer to a specific type.
 *
 * Convenience macro that unwraps the Box and casts the result to the
 * given type. Equivalent to `(type*)box_unwrap(box)`.
 *
 * @param box  Pointer to the Box.
 * @param type  Target type to cast to.
 * @return      The wrapped pointer cast to the specified type.
 */
#define unwrap(box, type) (type*)box_unwrap(box)
