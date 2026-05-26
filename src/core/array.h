/**
 * @file array.h
 * @brief Generic dynamic array implementation.
 *
 * Provides a type-erased, allocator-backed dynamic array with bounds-checked
 * access, insertion, and iteration support. All arrays are managed through
 * a shared Allocator for consistent memory handling.
 */

#pragma once

#include "core/allocator.h"
#include "core/panic.h"

/**
 * @struct Array
 * @brief Opaque, allocator-backed dynamic array.
 *
 * Elements are stored contiguously in memory. The array grows its
 * underlying buffer as needed via the associated allocator.
 */
typedef struct {
  /** Pointer to the contiguous element storage. */
  void* data;
  /** Number of elements currently stored. */
  usize length;
  /** Size of each element in bytes. */
  usize item_size;
} Array;

/**
 * @struct ArrayIter
 * @brief Iterator over an Array.
 *
 * Tracks the current index during sequential traversal. The iterator
 * does not own the array and does not validate bounds on access.
 */
typedef struct {
  /** Pointer to the array being iterated. */
  Array* arr;
  /** Current index into the array. */
  usize idx;
} ArrayIter;

/**
 * @brief Compute the number of elements in a static array at compile time.
 *
 * @param x  The array expression.
 * @return   The number of elements in x.
 */
#define array_len(x) (sizeof(x) / sizeof((x)[0]))

// #define array_create(type, size, values)

/**
 * @brief Initialize a heap-allocated Array with the given parameters.
 *
 * Allocates the backing buffer through the provided allocator. The
 * array starts with zero length and the specified capacity.
 *
 * @param self       Pointer to the Array to initialize.
 * @param alloc      Allocator to use for buffer allocation.
 * @param item_size  Size of each element in bytes.
 * @param capacity   Initial number of elements the buffer can hold.
 */
void array_init(Array* self, Allocator* alloc, usize item_size, usize capacity);

/**
 * @brief Initialize an Array without external allocator management.
 *
 * Similar to array_init() but does not associate an allocator. The
 * caller is responsible for managing the buffer's lifetime.
 *
 * @param self       Pointer to the Array to initialize.
 * @param item_size  Size of each element in bytes.
 * @param capacity   Initial number of elements the buffer can hold.
 */
void array_init_ce(Array* self, usize item_size, usize capacity);

/**
 * @brief Internal helper to get a pointer to an element at a given index.
 *
 * Checks bounds and panics if the index is out of range.
 *
 * @param self   Pointer to the Array.
 * @param index  Index of the element to retrieve.
 * @return       Pointer to the element at the given index, or NULL on error.
 */
static void* _array_get_impl(Array* self, const usize index) {
  if (index > self->length) {
    panic("vector_get on out of bounds index");
  }

  return (u8*)self->data + (index * self->item_size);
}

/**
 * @brief Construct an Array from a raw pointer and its compile-time size.
 *
 * Creates a non-owning Array view over existing memory. The caller
 * must ensure the pointed-to memory remains valid for the Array's lifetime.
 *
 * @param arr  The raw pointer to the array data.
 * @return     An Array struct referencing the given data.
 */
#define array_from_ptr(arr)                                                                                            \
  (Array) { .data = arr, .length = sizeof(arr) / sizeof(arr[0]), .item_size = sizeof(arr[0]) }

/**
 * @brief Get an element from an Array with type casting and bounds checking.
 *
 * @param arr    Pointer to the Array.
 * @param type   The element type to cast the result to.
 * @param index  Index of the element to retrieve.
 * @return       The element at the given index, cast to the specified type.
 */
#define array_get(arr, type, index) *(type*)_array_get_impl(arr, index)

/**
 * @brief Insert an element at a given index in the Array.
 *
 * Shifts existing elements to make room. The buffer is grown if
 * necessary via the associated allocator.
 *
 * @param self   Pointer to the Array.
 * @param index  Index at which to insert.
 * @param item   Pointer to the element to insert.
 */
void array_insert(Array* self, const usize index, const void* item);

/**
 * @brief Allocate and initialize a new heap-allocated Array.
 *
 * @param alloc      Allocator to use for buffer allocation.
 * @param item_size  Size of each element in bytes.
 * @param capacity   Initial number of elements the buffer can hold.
 * @return           A newly allocated Array on success, or NULL on failure.
 */
Array* array_new(Allocator* alloc, usize item_size, usize capacity);
