#ifndef ELX_TOKEN_H
#define ELX_TOKEN_H

#include <stdint.h>
#include <stdlib.h>

#include "allocator.h"
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

Token* token_new(Allocator* allocator);

// void token_free(token_t* self);

void token_init(Allocator* allocator, Token* self);

// void token_deinit(token_t* self);

Token* token_copy(Allocator* allocator, Token* self);

char* token_string(Allocator* allocator, Token* self);

#endif
