#include "token.h"
#include "fmt.h"
#include "str.h"
#include "xalloc.h"

Token* token_new(Allocator* allocator) {
  Token* token = allocator_alloc(allocator, sizeof(Token));
  return token;
}

void token_init(Allocator* allocator, Token* self) {
  xnotnull(self);

  *self = (Token){
      .kind = TOK_INVALID,
      .loc = 0,
      .size = 0,
  };

  string_init(allocator, &self->str);
}

Token* token_copy(Allocator* allocator, Token* self) {
  xnotnull(self);

  Token* token = allocator_alloc(allocator, sizeof(Token));

  *token = (Token){
      .kind = self->kind,
      .str = string_copy(allocator, &self->str),
      .loc = self->loc,
      .size = self->size,
  };

  return token;
}

char* token_string(Allocator* allocator, Token* self) {
  xnotnull(self);

  char* str = fmt(allocator, "Token(str=\"%s\", kind=\"%s\")", self->str.data,
                  token_kind_str(self->kind));

  return str;
}
