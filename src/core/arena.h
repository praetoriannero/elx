#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/modprim.h"

// Arena allocator with a global constexpr default size.
// To change the default size, redefine ARENA_DEFAULT_SIZE before including this header.
#define ARENA_DEFAULT_SIZE (1 << 20) // 1 MiB default arena size

// Arena chunk for doubly-linked list management
typedef struct ArenaChunk {
  struct ArenaChunk* prev; // Pointer to previous chunk (NULL for first chunk)
  struct ArenaChunk* next; // Pointer to next chunk (NULL for last chunk)
  void* buffer;            // Pointer to start of this chunk's memory
  usize size;              // Size of this chunk (ARENA_DEFAULT_SIZE)
  usize used;              // Amount currently used in this chunk
} ArenaChunk;

typedef struct Arena {
  ArenaChunk* chunks;        // Pointer to first chunk in the list
  ArenaChunk* current_chunk; // Pointer to current chunk being allocated from
  usize total_alloc;         // Total allocated bytes across all chunks
} Arena;

/**
 * Initialize an Arena with optional custom chunk size.
 *
 * Creates the first memory chunk for this Arena. If @p init_size is 0,
 * a default chunk of @c ARENA_DEFAULT_SIZE (typically 1 MiB) will be used.
 * The arena will automatically grow by adding new chunks when memory is exhausted.
 *
 * @param self Pointer to Arena struct to initialize
 * @param init_size Optional initial chunk size in bytes. If 0, uses ARENA_DEFAULT_SIZE.
 *
 * @note This function must be called once per Arena before using arena_alloc() or
 *       other allocation functions. Subsequent calls reset existing chunks.
 */
void arena_init(Arena* self, usize init_size);

/**
 * Free an individual allocation (no-op in arena mode).
 *
 * In arena allocator mode, this function does nothing because the arena keeps
 * all allocations alive until arena_deinit() is called. Use this only if you
 * have a custom implementation that tracks freed allocations.
 *
 * @param arena Pointer to Arena
 * @param ptr Allocation pointer to free (will be ignored in standard arena mode)
 */
void arena_free(Arena* arena, void* ptr);

/**
 * Allocate memory from an Arena chunk pool.
 *
 * Returns the next available pointer within the current chunk, or creates
 * a new chunk if no space exists in any existing chunks. The returned pointer
 * remains valid until freed by arena_free() or deallocated via arena_deinit().
 *
 * @param self Pointer to initialized Arena
 * @param size Number of bytes to allocate
 *
 * @return Pointer to newly allocated memory, or NULL on failure
 *
 * @warning All allocations are zero-initialized only in debug builds (ELX_DEBUG).
 *          In release builds, memory is not cleared for performance.
 */
void* arena_alloc(Arena* self, usize size);

/**
 * Reallocate an existing allocation to a new size.
 *
 * Attempts to keep the same pointer if there's space immediately after the
 * current allocation; otherwise allocates new space and copies data.
 *
 * @param self Pointer to Arena
 * @param old_ptr Existing allocation pointer (NULL for fresh allocation)
 * @param new_size New size in bytes
 *
 * @return New pointer to allocated memory. May be different from old_ptr.
 */
void* arena_realloc(Arena* self, void* old_ptr, usize new_size);

/**
 * Move an allocation from one arena to another.
 *
 * Finds the specified allocation in @p src and allocates new space in @p dst
 * of the same size, copying all data. The original pointer is marked as freed
 * back to the source arena's pool for future reuse.
 *
 * @param src Source Arena containing the allocation to move
 * @param dst Destination Arena to allocate into
 * @param ptr Allocation pointer in src to move to dst
 *
 * @note The moved allocation cannot be accessed via the original ptr after this call;
 *       use the new pointer from the destination arena instead.
 */
void arena_move(Arena* src, Arena* dst, void* ptr);

/**
 * Free all chunks and deallocate arena memory.
 *
 * Must be called when done with the Arena to prevent memory leaks. This
 * frees both chunk metadata and all buffer allocations. The Arena struct
 * itself (if on stack) will be automatically cleaned up by the compiler.
 *
 * @param self Pointer to Arena to deallocate
 */
void arena_deinit(Arena* self);

/**
 * Resize an allocation (alias for arena_alloc).
 *
 * In arena mode, this allocates a new buffer and returns it. The old
 * pointer remains in the pool if arena semantics are followed.
 *
 * @param alloc Arena to allocate from
 * @param old_ptr Existing pointer (ignored - always allocates fresh)
 * @param new_size New size in bytes
 *
 * @return Pointer to newly allocated memory
 */
void* arena_resize(Arena* alloc, void* old_ptr, usize new_size);

/**
 * Delete a pointer (no-op in arena mode).
 *
 * In arena mode, this function does nothing to support API compatibility.
 * All allocations live until arena_deinit() is called.
 *
 * @param alloc Arena containing the pointer
 * @param ptr Pointer to delete (will be ignored)
 */
void arena_delete(Arena* alloc, void* ptr);

/**
 * Copy data from one pointer into a new Arena allocation.
 *
 * Allocates new memory in @p alloc and copies @size bytes from @p ptr.
 *
 * @param alloc Arena to allocate destination buffer in
 * @param src Pointer to read source data from
 * @param size Number of bytes to copy
 *
 * @return Pointer to newly allocated copied data
 */
void* arena_copy(Arena* alloc, void* ptr, usize size);

// void* resize(Arena* alloc, void* old_ptr, usize new_size);

// void delete(Arena* alloc, void* ptr);

// void move(Arena* src, Arena* dst, void* ptr);

// void* copy(Arena* alloc, void* ptr, usize size);
