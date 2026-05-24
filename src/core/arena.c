#include <stdint.h>
#include <string.h>

#include "core/arena.h"
#include "core/panic.h"
#include "core/xalloc.h"

// Arena allocator with growing chunks via doubly-linked list.
// When the arena runs out of space, it automatically grows by adding another chunk of ARENA_DEFAULT_SIZE.

void arena_init(Arena* self, usize init_size) {
  xnotnull(self);

  // If already initialized (chunks exist), just reset pointers to start fresh
  if (self->chunks != NULL) {
    // Reuse existing chunks - just reset total_alloc and mark as new
    ArenaChunk* chunk = self->chunks;
    while (chunk != NULL) {
      chunk->used = 0;
      chunk = chunk->next;
    }
    self->total_alloc = 0;
  }
  // If not initialized, allocate a new first chunk with padding
  else {
    usize padded_size = (init_size > 0) ? init_size + sizeof(usize) : ARENA_DEFAULT_SIZE + sizeof(usize);

    void* ptr = xmalloc(padded_size);
    memset(ptr, 0, padded_size);

    // Create first chunk and add to doubly-linked list
    ArenaChunk* new_chunk = xmalloc(sizeof(ArenaChunk));
    new_chunk->prev = NULL; // First chunk has no previous
    new_chunk->next = NULL; // Will be set when linked
    new_chunk->buffer = ptr;
    new_chunk->size = ARENA_DEFAULT_SIZE; // Usable size only
    new_chunk->used = 0;

    self->chunks = new_chunk;
    self->current_chunk = new_chunk;
    self->total_alloc = 0;
  }
}

static ArenaChunk* arena_create_new_chunk(Arena* self) {
  xnotnull(self);

  // Allocate new chunk
  ArenaChunk* new_chunk = xmalloc(sizeof(ArenaChunk));
  ArenaChunk* last_chunk = self->chunks;

  // Find the last chunk in the list
  while (last_chunk->next != NULL) {
    last_chunk = last_chunk->next;
  }

  // Set up new chunk with small padding to avoid buffer overflow issues
  usize padded_size = ARENA_DEFAULT_SIZE + sizeof(usize); // Add a bit of headroom for alignment/padding
  new_chunk->prev = last_chunk;                           // Link to previous chunk
  new_chunk->next = NULL;                                 // Last chunk has no next
  new_chunk->buffer = xmalloc(padded_size);
  memset(new_chunk->buffer, 0, padded_size);
  new_chunk->size = ARENA_DEFAULT_SIZE;
  new_chunk->used = 0;

  // Link new chunk to the list
  last_chunk->next = new_chunk; // Previous chunk's next points to new

  return new_chunk;
}

void* arena_alloc(Arena* self, usize size) {
  xnotnull(self);
  xnotnull(self->chunks);

  // Find a chunk with enough space
  ArenaChunk* chunk = self->current_chunk;

  while (chunk != NULL) {
    if (chunk->size - chunk->used >= size) {
      // Found a chunk with enough space

      // Return pointer to next available space in this chunk
      void* ptr = (void*)((char*)chunk->buffer + chunk->used);

      // Update used count
      chunk->used += size;

#ifdef ELX_DEBUG
      memset(ptr, 0, size); // Zero-initialize for debug builds
#endif

      return ptr;
    }

    // Move to next chunk if available
    chunk = chunk->next;
  }

  // No chunk has enough space - add a new one
  ArenaChunk* new_chunk = arena_create_new_chunk(self);

  // Return pointer from the newly added chunk
  void* ptr = (void*)((char*)new_chunk->buffer + new_chunk->used);

#ifdef ELX_DEBUG
  memset(ptr, 0, size); // Zero-initialize for debug builds - must be before updating used!
#endif
  new_chunk->used += size;

  return ptr;
}

void* arena_realloc(Arena* self, void* old_ptr, usize new_size) {
  xnotnull(old_ptr);
  xnotnull(self);

  if (new_size == 0) {
    // Special case: zero-size allocation - return same pointer
    ArenaChunk* chunk = self->chunks;
    while (chunk != NULL) {
      usize chunk_end_offset = (usize)((char*)chunk->buffer + chunk->used - (char*)chunk->buffer);
      if ((void*)((char*)chunk->buffer + chunk_end_offset) == old_ptr) {
        return old_ptr; // Return same pointer
      }
      chunk = chunk->next;
    }
    panic("realloc: old pointer not found in arena\n");
  }

  if (old_ptr == NULL) {
    return arena_alloc(self, new_size);
  }

  // Find which chunk contains this allocation by scanning chunks
  ArenaChunk* old_chunk = self->chunks;
  usize alloc_offset = 0; // Offset within the chunk
  while (old_chunk != NULL) {
    // The allocations in a chunk are at positions from 0 to used
    // So we check if old_ptr falls within this chunk's allocated range
    if ((char*)old_ptr >= (char*)old_chunk->buffer && (char*)old_ptr < (char*)old_chunk->buffer + old_chunk->used) {
      break;
    }
    old_chunk = old_chunk->next;
  }

  if (old_chunk == NULL) {
    panic("realloc: old pointer not found in arena\n");
  }

  // Calculate the actual offset within this chunk
  alloc_offset = (usize)((char*)old_ptr - (char*)old_chunk->buffer);

  // Try to grow in place if possible, or find space elsewhere
  usize space_after_alloc = old_chunk->size - alloc_offset;

  if (space_after_alloc >= new_size) {
    return old_ptr; // No need to move - room after allocation
  }

  // Allocate new space and copy data
  void* new_ptr = arena_alloc(self, new_size);

  // Copy the existing data from old location to new location
  usize actual_data_to_copy = old_chunk->used - alloc_offset; // Bytes from this allocation onwards
  memcpy(new_ptr, old_ptr, actual_data_to_copy < new_size ? actual_data_to_copy : new_size);

  return new_ptr;
}

void arena_free(Arena* arena, void* ptr) {
  // Arena allocators don't typically free individual allocations
  // Individual pointers remain in the pool until arena_deinit()
  xnotnull(ptr);
}

void arena_move(Arena* src, Arena* dst, void* ptr) {
  xnotnull(src);
  xnotnull(dst);

  // Find which chunk contains this allocation and its offset
  ArenaChunk* old_chunk = NULL;
  usize alloc_offset = 0;

  while (src->chunks != NULL) {
    if ((char*)ptr >= (char*)src->chunks->buffer && (char*)ptr < (char*)src->chunks->buffer + src->chunks->used) {
      old_chunk = src->chunks;
      alloc_offset = (usize)((char*)ptr - (char*)src->chunks->buffer);
      break;
    }
    src->chunks = src->chunks->next;
  }

  if (old_chunk == NULL) {
    panic("arena_move: source pointer not found in arena\n");
  }

  // Calculate total data to copy (from allocation start to end of chunk or current used, whichever is smaller)
  usize data_to_copy = old_chunk->used - alloc_offset;

  // Allocate new space in destination arena
  void* new_ptr = arena_alloc(dst, data_to_copy);

  // Copy the data
  memcpy(new_ptr, ptr, data_to_copy);

  // Mark old allocation as freed (conceptually) by advancing used past it
  old_chunk->used = alloc_offset; // Effectively frees this allocation back to pool
}

void arena_deinit(Arena* self) {
  if (self) {
    // Free all chunks and deallocate both chunk metadata AND their buffers
    ArenaChunk* chunk = self->chunks;

    // Walk through the list, freeing each chunk
    while (chunk != NULL) {
      ArenaChunk* next = chunk->next;
      xfree(chunk->buffer); // Free the buffer allocated in arena_init
      xfree(chunk);         // Free the chunk metadata
      chunk = next;
    }
  }
}

// Wrapper functions matching the original allocator API (prefixed with arena_)
void* arena_new(Arena* alloc, usize size) { return arena_alloc(alloc, size); }

void* arena_resize(Arena* alloc, void* old_ptr, usize new_size) {
  // Arena realloc doesn't work well - allocate new and copy
  if (old_ptr) {
    // The old pointer stays in the pool (arena semantics)
    // Just return a new allocation instead
    return arena_alloc(alloc, new_size);
  }
  return arena_alloc(alloc, new_size);
}

void arena_delete(Arena* alloc, void* ptr) {
  // Arena allocator doesn't free back to system by default
  // Keep allocations in the pool until arena_deinit()
  // This is for API compatibility - actual freeing requires custom handling
  xnotnull(ptr);
}

void* arena_copy(Arena* alloc, void* ptr, usize size) {
  void* ret_ptr = arena_alloc(alloc, size);
  memcpy(ret_ptr, ptr, size);
  return ret_ptr;
}
