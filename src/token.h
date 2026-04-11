#ifndef ELX_TOKEN_H
#define ELX_TOKEN_H

#include <stdint.h>
#include <stdlib.h>

#include "arena.h"
#include "str.h"
#include "token_kind.h"

typedef struct token {
  TokenKind kind;
  String str;
  usize start;
  usize end;
} Token;

typedef struct {
  Token* tokens;
  uint32_t token_count;
} token_array_t;

Token* token_new(Arena* arena);

// void token_free(token_t* self);

void token_init(Arena* arena, Token* self);

// void token_deinit(token_t* self);

Token* token_copy(Arena* arena, Token* self);

char* token_string(Arena* arena, Token* self);

#endif
