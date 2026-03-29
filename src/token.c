#include "token.h"
#include "fmt.h"
#include "str.h"
#include "xalloc.h"

Token* token_new(Arena* arena) {
    Token* token = arena_alloc(arena, sizeof(Token));
    token->init(arena, token);
    return token;
}

void token_init(Arena* arena, Token* self) {
    xnotnull(self);

    *self = (Token){
        .kind = TOK_INVALID,
        .length = 0,
        .line = 0,
        .column = 0,

        .init = token_init,
        .copy = token_copy,
        .string = token_string,
    };

    string_init(arena, &self->str);
}

Token* token_copy(Arena* arena, Token* self) {
    xnotnull(self);

    Token* token = arena_alloc(arena, sizeof(Token));

    *token = (Token){
        .kind = self->kind,
        .str = string_copy(arena, &self->str),
        .length = self->length,
        .line = self->line,
        .column = self->column,
    };

    return token;
}

char* token_string(Arena* arena, Token* self) {
    xnotnull(self);

    char* str = fmt(arena, "Token(str=\"%s\", kind=\"%s\")", self->str.data, token_kind_str(self->kind));

    return str;
}
