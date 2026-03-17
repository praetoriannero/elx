#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "arena.h"
// #include "str.h"
#include "token.h"
#include "token_kind.h"

typedef struct {
    size_t loc;
    size_t length;
    size_t line;
    size_t col;
    size_t last_col;
} lexer_meta_t;

typedef struct lexer {
    char* data;
    // string_t* token_string;
    lexer_meta_t meta;
} lexer_t;

// int64_t lexer_meta_str(lexer_t* stream, char* meta_str);

void lexer_init(lexer_t* self, char* data);

token_t lexer_next(arena_t* arena, lexer_t* self);

token_t lexer_peek(arena_t* arena, lexer_t* self);

i64 lexer_scan(lexer_t* self, token_kind_t key);

char lexer_peek_char(lexer_t* stream);

char lexer_consume(lexer_t* stream);

void lexer_deinit(lexer_t* stream);
