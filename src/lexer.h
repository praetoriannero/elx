#pragma once

#include <stdint.h>

#include "allocator.h"
#include "token.h"
#include "token_kind.h"

typedef struct {
  usize loc;
  usize length;
  usize line;
  usize col;
} LexerContext;

typedef struct lexer {
  const char* data;
  const char* file_name;
  LexerContext context;
  Allocator* alloc;
} Lexer;

void lexer_init(Lexer* self, Allocator* alloc, const char* data, const char* file_name);

Token lexer_next(Lexer* self);

Token lexer_peek(Lexer* self);

i64 lexer_scan(Lexer* self, TokenKind key);

char lexer_peek_first(Lexer* self);

char lexer_peek_second(Lexer* self);

char lexer_peek_last(Lexer* self);

char lexer_consume(Lexer* stream);

void lexer_deinit(Lexer* stream);

void lexer_consume_into(Lexer* self, String* string, usize count);
