#include "arena.h"
#include "panic.h"
#include "xalloc.h"
#include <stdint.h>


void arena_reset(arena_t* self);

void arena_free(arena_t* self);

void* arena_alloc(arena_t* self, size_t size) {
    if ((self->ptr + size) > self->block_end) {
        panic("arena exhausted block size; failed to allocate %zu bytes", size);
    }

    void* ret_ptr = (void*)self->ptr;
    self->ptr += size;

    return ret_ptr;
}

arena_t* arena_new(uint64_t block_size) {
    arena_t* arena = xmalloc(sizeof(arena_t));
    arena_init(arena, block_size);
    return arena;
}

void arena_init(arena_t* self, uint64_t block_size) {
    void* mem_base = xmalloc(block_size);
    *self = (arena_t){
        .block_head = mem_base,
        .block_end = (uintptr_t)mem_base + block_size,
        .block_size = block_size,
        .ptr = (uintptr_t)mem_base,
    };
}

void arena_deinit(arena_t* self) {
    xfree(self->block_head);
    xfree(self);
}
