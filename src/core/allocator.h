/**
 * @file allocator.h
 * @brief Memory allocator with scope-based lifetime management.
 *
 * Provides a flexible memory allocator that supports both manual
 * allocation/deallocation and scoped (arena-style) memory management.
 * Allocators can be nested to create scope hierarchies, where freeing
 * a scope automatically deallocates all memory allocated within it.
 *
 * Key features:
 *   - Custom alloc/realloc/free function pointers for flexibility
 *   - Scoped allocation for automatic cleanup
 *   - Memory tracking (total allocated bytes)
 *   - Helper macros for scoped allocators
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/modprim.h"

/**
 * @brief A node in the allocator's scope tree.
 *
 * Each node represents a scope with its own memory buffer.
 * Nodes form a tree via parent/child links.
 */
typedef struct __attribute__((aligned(sizeof(max_align_t)))) AllocatorNode {
  struct AllocatorNode* parent; ///< Parent scope node
  struct AllocatorNode* child;  ///< Child scope node
  usize size;                   ///< Size of the allocated buffer
  void* buffer;                 ///< Pointer to the allocated memory buffer
} AllocatorNode;

/**
 * @brief Alias for AllocatorNode representing a scope.
 */
typedef AllocatorNode AllocatorScope;

/**
 * @brief A memory allocator with custom allocation functions.
 *
 * Manages memory allocation through function pointers, supporting
 * both individual allocations and scoped allocation trees.
 */
typedef struct Allocator {
  AllocatorNode* node_end;           ///< Last allocated node in the scope tree
  usize total_alloc;                 ///< Total bytes allocated across all scopes
  void* (*alloc)(struct Allocator*, usize);   ///< Custom allocation function
  void* (*realloc)(struct Allocator*, void*, usize); ///< Custom reallocation function
  void (*free)(struct Allocator*, void*);      ///< Custom deallocation function
} Allocator;

/**
 * @brief Create a new root allocator.
 *
 * @return Pointer to the newly allocated Allocator
 */
Allocator* allocator_new(void);

/**
 * @brief Free a specific allocation from an allocator.
 *
 * @param allocator Allocator to free from
 * @param ptr Pointer to the memory to free
 */
void allocator_free(Allocator* allocator, void* ptr);

/**
 * @brief Create a new scope (child) within an allocator.
 *
 * @param self Pointer to the parent allocator
 * @return Pointer to the newly created scope
 */
AllocatorScope* allocator_new_scope(Allocator* self);

/**
 * @brief Free a scope and all memory allocated within it.
 *
 * @param self Pointer to the parent allocator
 * @param scope Pointer to the scope to free
 */
void allocator_free_scope(Allocator* self, AllocatorScope* scope);

/**
 * @brief Allocate memory from an allocator.
 *
 * @param self Pointer to the allocator
 * @param size Number of bytes to allocate
 * @return Pointer to the allocated memory
 */
void* allocator_alloc(Allocator* self, usize size);

/**
 * @brief Reallocate memory from an allocator.
 *
 * @param self Pointer to the allocator
 * @param old_ptr Pointer to the existing memory
 * @param new_size New size in bytes
 * @return Pointer to the reallocated memory
 */
void* allocator_realloc(Allocator* self, void* old_ptr, usize new_size);

/**
 * @brief Deinitialize and free all nodes in an allocator.
 *
 * @param self Pointer to the allocator to deinitialize
 */
void allocator_deinit(Allocator* self);

/**
 * @brief Initialize an allocator with default settings.
 *
 * @param self Pointer to the allocator to initialize
 */
void allocator_init(Allocator* self);

/**
 * @brief Move an allocation from one allocator to another.
 *
 * @param lhs Destination allocator
 * @param rhs Source allocator
 * @param ptr Pointer to the memory to move
 */
void allocator_move(Allocator* lhs, Allocator* rhs, void* ptr);

/**
 * @brief Macro to create a scoped allocator with automatic cleanup.
 *
 * Declares a local allocator that is automatically deinitialized
 * when it goes out of scope.
 *
 * @param name Name of the allocator variable
 */
#define scoped_allocator(name)                                                                                         \
  __attribute__((__cleanup__(allocator_deinit))) Allocator name = {};                                                  \
  allocator_init(&name);

/**
 * @brief Allocate memory using the new operator style.
 *
 * @param alloc Allocator to use
 * @param size Number of bytes to allocate
 * @return Pointer to the allocated memory
 */
void* new (Allocator* alloc, usize size);

/**
 * @brief Reallocate memory using the resize operator style.
 *
 * @param alloc Allocator to use
 * @param old_ptr Pointer to existing memory
 * @param new_size New size in bytes
 * @return Pointer to the reallocated memory
 */
void* resize(Allocator* alloc, void* old_ptr, usize new_size);

/**
 * @brief Free memory using the delete operator style.
 *
 * @param alloc Allocator that owns the memory
 * @param ptr Pointer to the memory to free
 */
void delete (Allocator* alloc, void* ptr);

/**
 * @brief Move memory using the move operator style.
 *
 * @param src Source allocator
 * @param dst Destination allocator
 * @param ptr Pointer to the memory to move
 */
void move(Allocator* src, Allocator* dst, void* ptr);

/**
 * @brief Transfer ownership of memory to an allocator using the into operator style.
 *
 * @param alloc Allocator to take ownership
 * @param ptr Pointer to the memory
 */
void into(Allocator* alloc, void* ptr);

/**
 * @brief Copy memory using the copy operator style.
 *
 * @param alloc Allocator to allocate the copy
 * @param ptr Pointer to the source memory
 * @param size Number of bytes to copy
 * @return Pointer to the newly allocated copy
 */
void* copy(Allocator* alloc, void* ptr, usize size);