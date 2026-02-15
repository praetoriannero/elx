#pragma once

#include <stdint.h>
#include <stdlib.h>


typedef struct {
    size_t* head;
    size_t* next;
} block_t;

typedef struct {
    block_t* block_head;
    uint64_t block_size;
    size_t ptr;
} arena_t;

void arena_reset(arena_t* self);

void arena_free(arena_t* self);

void* arena_alloc(arena_t* self, size_t size);

arena_t* arena_new(uint64_t block_size);

void arena_init(arena_t* self, uint64_t block_size);

void arena_deinit(arena_t* self);
