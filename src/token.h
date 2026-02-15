#ifndef ELX_TOKEN_H
#define ELX_TOKEN_H

#include <stdint.h>
#include <stdlib.h>

#include "str.h"
#include "token_kind.h"

typedef struct token {
    token_kind_t kind;
    string_t* str;
    size_t length;
    size_t line;
    size_t column;

    void (*init)(struct token*);
    void (*deinit)(struct token*);
    struct token* (*copy)(struct token*);
    char* (*string)(struct token*);
} token_t;

typedef struct {
    token_t* tokens;
    uint32_t token_count;
} token_array_t;

void token_init(token_t* self);

void token_deinit(token_t* self);

token_t* token_copy(token_t* self);

char* token_string(token_t* self);

#endif
