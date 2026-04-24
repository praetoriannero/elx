#ifndef ELX_TOKEN_H
#define ELX_TOKEN_H

#include <stdint.h>

#include "core/allocator.h"
#include "core/str.h"
#include "token/token_kind.h"

typedef struct token {
  TokenKind kind;
  String str;
  usize loc;
  usize size;
  struct {
    BaseKind base_kind;
    const char* base_digits;
    usize suffix_start;
  } number;
} Token;

Token* token_new(Allocator* allocator);

void token_init(Allocator* allocator, Token* self);

Token* token_copy(Allocator* allocator, Token* self);

char* token_string(Allocator* allocator, Token* self);

#endif
