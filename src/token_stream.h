#ifndef ELX_TOKEN_STREAM_H
#define ELX_TOKEN_STREAM_H

#include <stdint.h>
#include <stdlib.h>

#include "str.h"
#include "token.h"


typedef struct token_stream {
    char* data;
    string_t* token_string;
    size_t loc;
    size_t length;
    size_t line;
    size_t col;
    size_t last_col;
} token_stream_t;

int64_t token_stream_meta_str(token_stream_t* stream, char* meta_str);

void token_stream_init(token_stream_t* self, char* data);

token_t token_stream_next(token_stream_t* stream);

char token_stream_peek(token_stream_t* stream);

char token_stream_peek_next(token_stream_t* self);

char token_stream_consume(token_stream_t* stream);

void token_stream_deinit(token_stream_t* stream);

#endif

