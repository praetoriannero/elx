#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "str.h"
#include "token.h"
#include "token_stream.h"
#include "token_utils.h"
#include "xalloc.h"

void token_stream_init(token_stream_t *self, char *data) {
    self->data = data;
    self->loc = 0;
    self->col = 0;
    self->last_col = 0;
    self->length = strlen(data);
    self->line = 0;

    string_t *string = xmalloc(sizeof(string_t));
    string_init(string);
    self->token_string = string;
}

// static void _handle_operator(token_stream_t* self, token_t* token, char c,
// string_t* token_str);
//
// static void _handle_string(token_stream_t* self, token_t* token, char c,
// string_t* token_str);
//
// static void _handle_comment(token_stream_t* self, token_t* token, char c,
// string_t* token_str);
//
// static void _handle_ident(token_stream_t* self, token_t* token, char c,
// string_t* token_str);

token_t *token_stream_next(token_stream_t *self) {
    char c;

    token_t *token = xmalloc(sizeof(token_t));
    token_init(token);

    string_t *token_str = xmalloc(sizeof(string_t));
    string_init(token_str);

    c = token_stream_consume(self);
    while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) {
        c = token_stream_consume(self);
        if (c == -1) {
            break;
        }
    }

    if (c == -1) {
        token->kind = TOK_EOF;
        return token;
    }

    token->kind = single_char_token[(uint8_t)c];

    bool check = true;

    if (check) {
        if (token->kind != TOK_INVALID) {
            char tmp;
            string_t tmp_str;
            token_kind_t next_kind;

            string_init(&tmp_str);
            string_push_char(token_str, c);

            c = token_stream_peek(self);
            tmp = c;
            next_kind = single_char_token[(uint8_t)c];

            while (next_kind != TOK_INVALID) {
                string_push_char(token_str, c);
                c = token_stream_consume(self);
                tmp = token_stream_peek(self);
                next_kind = single_char_token[(uint8_t)tmp];
            }
            check = false;
        }
    }

    if (check) {
        if (is_ident_char(c)) {
            while (is_ident_char(c)) {
                string_push_char(token_str, c);
                if (!is_ident_char(token_stream_peek(self))) {
                    break;
                }
                c = token_stream_consume(self);
            }
            token->kind = TOK_IDENT;
            check = false;
        }
    }

    token->str = string_copy(token_str);

    string_deinit(token_str);
    return token;
}

int64_t token_stream_meta_str(token_stream_t *self, char *meta_str) {
    return sprintf(
        meta_str,
        "token_stream_t{token=%s, loc=%zu, length=%zu, line=%zu, col=%zu}",
        self->token_string->data, self->loc, self->length, self->line + 1,
        self->col + 1);
}

void token_stream_reset(token_stream_t *self) {
    string_clear(self->token_string);
    self->loc = 0;
    self->line = 0;
    self->col = 0;
    self->last_col = 0;
}

char token_stream_peek(token_stream_t *self) { return self->data[self->loc]; }

char token_stream_peek_next(token_stream_t *self) {
    if (self->loc == self->length - 1) {
        return -1;
    }

    return self->data[self->loc + 1];
}

char token_stream_consume(token_stream_t *self) {
    if (self->loc == self->length - 1) {
        return -1;
    }

    char c = self->data[self->loc++];

    self->last_col = self->col;

    if (c == '\n') {
        self->line++;
        self->col = 0;
    } else {
        self->col++;
    }

    return c;
}

void token_stream_deinit(token_stream_t *self) {
    string_deinit(self->token_string);
    xfree(self);
}
