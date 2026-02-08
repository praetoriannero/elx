#include "str.h"
#include "token.h"
#include "xalloc.h"

void token_init(token_t* self) {
    string_t* str = xmalloc(sizeof(string_t));

    self->kind = TOK_INVALID;
    self->str = str;
    self->length = 0;
    self->line = 0;
    self->column = 0;
}

void token_deinit(token_t* self) {
    string_deinit(self->str);
    free(self);
}

token_t* token_copy(token_t* self) {
    token_t* token = xmalloc(sizeof(token_t));

    token->kind = self->kind;
    token->str = string_copy(self->str);
    token->length = self->length;
    token->line = self->line;
    token->column = self->column;

    return token;
}

