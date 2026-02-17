#include "token.h"
#include "fmt.h"
#include "str.h"
#include "xalloc.h"

token_t* token_new(void) {
    token_t* token = new (token_t);
    token->init(token);
    return token;
}

void token_free(token_t* self) {
    if (!self)
        return;
    token_deinit(self);
    xfree(self);
}

void token_init(token_t* self) {
    xnotnull(self);

    string_t str;
    string_init(&str);

    *self = (token_t){
        .kind = TOK_INVALID,
        .str = str,
        .length = 0,
        .line = 0,
        .column = 0,

        .init = token_init,
        .deinit = token_deinit,
        .copy = token_copy,
        .string = token_string,
    };
}

void token_deinit(token_t* self) {
    if (!self) {
        return;
    }

    string_deinit(&self->str);

    self->kind = TOK_INVALID;
    self->length = 0;
    self->line = 0;
    self->column = 0;
}

token_t* token_copy(token_t* self) {
    xnotnull(self);

    token_t* token = new (token_t);

    *token = (token_t){
        .kind = self->kind,
        .str = string_copy(&self->str),
        .length = self->length,
        .line = self->line,
        .column = self->column,
    };

    return token;
}

char* token_string(token_t* self) {
    xnotnull(self);

    char* str = NULL;
    str = fmt("Token(str=\"%s\", kind=\"%s\")", self->str.data,
              token_kind_str(self->kind));

    return str;
}
