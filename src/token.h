#ifndef ELX_TOKEN_H
#define ELX_TOKEN_H

#include <stdlib.h>

#include "str.h"
#include "token_kind.h"


typedef struct token {
    token_kind_t kind;
    string_t* str;
    size_t length;
    size_t line;
    size_t column;
} token_t;

void token_init(token_t* self);

void token_deinit(token_t* self);

token_t* token_copy(token_t* self);

#endif

