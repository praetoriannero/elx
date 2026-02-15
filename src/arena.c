#include "arena.h"
#include "xalloc.h"
#include <stdint.h>


void arena_reset(arena_t* self);

void arena_free(arena_t* self);

void* arena_alloc(arena_t* self, size_t size) {


}

arena_t* arena_new(uint64_t block_size) {
    arena_t* arena = xmalloc(sizeof(arena_t));
    arena_init(arena, block_size);
    return arena;
}

void arena_init(arena_t* self, uint64_t block_size) {
    void* mem_base = xmalloc(block_size);

    self->block_size = block_size;
    self->block_head = mem_base;
    self->ptr = (size_t)mem_base;
}

void arena_deinit(arena_t* self) {
    block_t* head = self->block_head;
    while (head) {

    }
}
