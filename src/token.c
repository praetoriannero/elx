#include "token.h"
#include "fmt.h"
#include "str.h"
#include "xalloc.h"

Token* token_new(Arena* arena) {
  Token* token = arena_alloc(arena, sizeof(Token));
  return token;
}

void token_init(Arena* arena, Token* self) {
  xnotnull(self);

  *self = (Token){
      .kind = TOK_INVALID,
      .start = 0,
      .end = 0,
  };

  string_init(arena, &self->str);
}

Token* token_copy(Arena* arena, Token* self) {
  xnotnull(self);

  Token* token = arena_alloc(arena, sizeof(Token));

  *token = (Token){
      .kind = self->kind,
      .str = string_copy(arena, &self->str),
      .start = self->start,
      .end = self->end,
  };

  return token;
}

char* token_string(Arena* arena, Token* self) {
  xnotnull(self);

  char* str = fmt(arena, "Token(str=\"%s\", kind=\"%s\")", self->str.data, token_kind_str(self->kind));

  return str;
}
