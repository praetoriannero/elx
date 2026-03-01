#ifndef ELX_TOKEN_H
#define ELX_TOKEN_H

#include <stdint.h>
#include <stdlib.h>

#include "arena.h"
#include "str.h"
#include "token_kind.h"

typedef struct token {
    token_kind_t kind;
    string_t str;
    size_t length;
    size_t line;
    size_t column;

    void (*init)(arena_t*, struct token*);
    // void (*deinit)(struct token*);
    struct token* (*copy)(arena_t*, struct token*);
    char* (*string)(arena_t*, struct token*);
} token_t;

typedef struct {
    token_t* tokens;
    uint32_t token_count;
} token_array_t;

token_t* token_new(arena_t* arena);

// void token_free(token_t* self);

void token_init(arena_t* arena, token_t* self);

// void token_deinit(token_t* self);

token_t* token_copy(arena_t* arena, token_t* self);

char* token_string(arena_t* arena, token_t* self);

#endif
