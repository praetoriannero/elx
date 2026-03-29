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
} LexerContext;

typedef struct lexer {
    char* data;
    LexerContext context;
} Lexer;

// int64_t lexer_meta_str(Lexer* stream, char* meta_str);

void lexer_init(Lexer* self, char* data);

Token lexer_next(Arena* arena, Lexer* self);

Token lexer_peek(Arena* arena, Lexer* self);

i64 lexer_scan(Lexer* self, TokenKind key);

char lexer_peek_char(Lexer* stream);

char lexer_consume(Lexer* stream);

void lexer_deinit(Lexer* stream);
