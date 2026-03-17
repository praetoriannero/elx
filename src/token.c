#include "token.h"
#include "fmt.h"
#include "str.h"
#include "xalloc.h"

token_t* token_new(arena_t* arena) {
    token_t* token = arena_alloc(arena, sizeof(token_t));
    token->init(arena, token);
    return token;
}

void token_init(arena_t* arena, token_t* self) {
    xnotnull(self);

    *self = (token_t){
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

token_t* token_copy(arena_t* arena, token_t* self) {
    xnotnull(self);

    token_t* token = arena_alloc(arena, sizeof(token_t));

    *token = (token_t){
        .kind = self->kind,
        .str = string_copy(arena, &self->str),
        .length = self->length,
        .line = self->line,
        .column = self->column,
    };

    return token;
}

char* token_string(arena_t* arena, token_t* self) {
    xnotnull(self);

    char* str = fmt(arena, "Token(str=\"%s\", kind=\"%s\")", self->str.data, token_kind_str(self->kind));

    return str;
}
