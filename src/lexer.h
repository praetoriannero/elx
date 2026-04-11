#pragma once

#include <stdint.h>

#include "arena.h"
#include "token.h"
#include "token_kind.h"

typedef struct {
  usize loc;
  usize length;
  usize line;
  usize col;
} LexerContext;

typedef struct lexer {
  char* data;
  LexerContext context;
} Lexer;

void lexer_init(Lexer* self, char* data);

Token lexer_next(Arena* arena, Lexer* self);

Token lexer_peek(Arena* arena, Lexer* self);

i64 lexer_scan(Lexer* self, TokenKind key);

char lexer_peek_char(Lexer* stream);

char lexer_consume(Lexer* stream);

void lexer_deinit(Lexer* stream);
