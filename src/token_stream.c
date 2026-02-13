#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "str.h"
#include "token.h"
#include "token_stream.h"
#include "token_kind.h"
#include "token_tables.h"
#include "token_utils.h"
#include "xalloc.h"

void token_stream_error(token_stream_t* self, char* msg) {
    panic("parsing error: %s at location %d:%d in file FILE\n", msg, self->line, self->col);
}

void token_stream_init(token_stream_t* self, char* data) {
    xnotnull(self);

    self->data = data;
    self->loc = 0;
    self->col = 0;
    self->last_col = 0;
    self->length = strlen(data);
    self->line = 0;

    string_t* string = xmalloc(sizeof(string_t));
    string_init(string);
    self->token_string = string;
}

static const char* whitespace = " \t\n\r";

static void token_stream_skip_whitespace(token_stream_t* self) {
    char c = token_stream_peek(self);
    while (strchr(whitespace, c)) {
        c = token_stream_consume(self);
        if (c == -1) {
            break;
        }
        c = token_stream_peek(self);
    }
}

static void token_stream_skip_comment(token_stream_t* self) {
    char c = token_stream_peek(self);
    while (c != '\n') {
        c = token_stream_consume(self);
        if (c == -1) {
            break;
        }
        c = token_stream_peek(self);
    }
}

token_t* token_stream_next(token_stream_t* self) {
    char c;

    token_t* token = xmalloc(sizeof(token_t));
    token_init(token);

    string_t* token_str = xmalloc(sizeof(string_t));
    string_init(token_str);

    token_stream_skip_whitespace(self);
    c = token_stream_consume(self);

    if (c == -1) {
        token->kind = TOK_EOF;
        return token;
    }

    token->kind = single_char_token[(uint8_t)c];

    // operator
    if (token->kind != TOK_INVALID) {
        string_t op_string;
        string_init(&op_string);
        string_push_char(&op_string, c);

        const op_node_t* op_iter = op_table;
        size_t table_size = array_len(op_table);
        op_node_t op_node;

        char c_next = token_stream_peek(self);
        token_kind_t kind_next = single_char_token[(uint8_t)c_next];

        for (size_t i = 0; i < table_size; i++) {
            op_node = op_iter[i];
            if (op_node.text == c) {
                op_iter = op_node.children;
                break;
            }
        }

        bool found = false;
        while (op_iter && (kind_next != TOK_INVALID) && (!strchr(whitespace, c_next))) {
            for (size_t i = 0; i < table_size; i++) {
                op_node = op_iter[i];
                if (op_node.text == c_next) {
                    string_push_char(&op_string, token_stream_consume(self));
                    op_iter = op_node.children;
                    token->kind = op_node.kind;
                    found = true;
                    break;
                }
            }

            if (!found) {
                break;
            }

            c_next = token_stream_peek(self);
            kind_next = single_char_token[(uint8_t)c_next];
        }

        token_str = string_copy(&op_string);
        string_clear(&op_string);

        goto fn_next_exit;
    }

    // identifier
    if (is_valid_ident_start(c)) {
        while (is_ident_char(c)) {
            string_push_char(token_str, c);
            if (!is_ident_char(token_stream_peek(self))) {
                break;
            }
            c = token_stream_consume(self);
        }

        size_t length = array_len(keyword_kind_table);
        for (uint8_t i = 0; i < length; i++) {
            string_token_t st = keyword_kind_table[i];
            if (strcmp(st.text, token_str->data) == 0) {
                token->kind = st.kind;
                goto fn_next_exit;
            }
        }
        token->kind = TOK_IDENT;
        goto fn_next_exit;
    }

    // integer/float
    if (isdigit(c)) {
        uint8_t decimal_count = 0;
        while (isdigit(c) || c == '.') {
            if (c == '.') {
                decimal_count++;
            }

            if (decimal_count > 1) {
                token_stream_error(self, "invalid integer format");
            }

            string_push_char(token_str, c);
            char c_next = token_stream_peek(self);
            if ((c_next != '.') && (!isdigit(c_next))) {
                break;
            }

            c = token_stream_consume(self);
        }

        if (decimal_count) {
            token->kind = TOK_FLOAT;
        } else {
            token->kind = TOK_INTEGER;
        }

        goto fn_next_exit;
    }

fn_next_exit:
    if (token->kind == TOK_COMMENT) {
        token_stream_skip_comment(self);
        token_stream_next(self);
    }

    token->str = string_copy(token_str);
    string_deinit(token_str);
    return token;
}

int64_t token_stream_meta_str(token_stream_t* self, char* meta_str) {
    return sprintf(
        meta_str,
        "token_stream_t{token=%s, loc=%zu, length=%zu, line=%zu, col=%zu}",
        self->token_string->data, self->loc, self->length, self->line + 1,
        self->col + 1);
}

void token_stream_reset(token_stream_t* self) {
    string_clear(self->token_string);
    self->loc = 0;
    self->line = 0;
    self->col = 0;
    self->last_col = 0;
}

char token_stream_peek(token_stream_t* self) { return self->data[self->loc]; }

char token_stream_peek_next(token_stream_t* self) {
    if (self->loc == self->length - 1) {
        return -1;
    }

    return self->data[self->loc + 1];
}

char token_stream_consume(token_stream_t* self) {
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

void token_stream_deinit(token_stream_t* self) {
    string_deinit(self->token_string);
    xfree(self);
}
