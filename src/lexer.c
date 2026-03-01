#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"
#include "array.h"
#include "lexer.h"
#include "logger.h"
#include "modprim.h"
#include "str.h"
#include "token.h"
#include "token_kind.h"
#include "token_tables.h"
#include "token_utils.h"
#include "xalloc.h"

void lexer_error(lexer_t* self, char* msg) {
    panic("lexing error: %s at location %d:%d in file FILE\n", msg,
          self->meta.line, self->meta.col);
}

void lexer_init(lexer_t* self, char* data) {
    xnotnull(self);

    self->data = data;
    self->meta.loc = 0;
    self->meta.col = 0;
    self->meta.last_col = 0;
    self->meta.length = strlen(data);
    self->meta.line = 0;
}

static const char* whitespace = " \t\n\r";

static void lexer_skip_whitespace(lexer_t* self) {
    char c = lexer_peek(self);
    while (strchr(whitespace, c)) {
        c = lexer_consume(self);
        if (c == -1) {
            break;
        }
        c = lexer_peek(self);
    }
}

static void lexer_skip_comment(lexer_t* self) {
    char c = lexer_peek(self);
    while (c != '\n') {
        c = lexer_consume(self);
        if (c == -1) {
            break;
        }
        c = lexer_peek(self);
    }
}

token_t lexer_next(arena_t* arena, lexer_t* self) {
    log("entering lexer_next\n");
    arena_t local_arena;
    arena_init(&local_arena);

    char c;

    token_t token;
    token_init(arena, &token);

    lexer_skip_whitespace(self);
    c = lexer_consume(self);

    if (c == -1) {
        token.kind = TOK_EOF;
        return token;
    }

    token.kind = single_char_token[(u8)c];

    // operator
    if (token.kind != TOK_INVALID) {
        string_push_char(arena, &token.str, c);

        const op_node_t* op_iter = op_table;
        size_t table_size = array_len(op_table);
        op_node_t op_node;

        char c_next = lexer_peek(self);
        token_kind_t kind_next = single_char_token[(u8)c_next];

        for (size_t i = 0; i < table_size; i++) {
            op_node = op_iter[i];
            if (op_node.text == c) {
                op_iter = op_node.children;
                break;
            }
        }

        bool found = false;
        while (op_iter && (kind_next != TOK_INVALID) &&
               (!strchr(whitespace, c_next))) {
            for (size_t i = 0; i < table_size; i++) {
                op_node = op_iter[i];
                if (op_node.text == c_next) {
                    string_push_char(arena, &token.str, lexer_consume(self));
                    op_iter = op_node.children;
                    token.kind = op_node.kind;
                    found = true;
                    break;
                }
            }

            if (!found) {
                break;
            }

            c_next = lexer_peek(self);
            kind_next = single_char_token[(u8)c_next];
        }

        goto fn_next_exit;
    }

    // identifier
    if (is_valid_ident_start(c)) {
        while (is_ident_char(c)) {
            string_push_char(arena, &token.str, c);
            if (!is_ident_char(lexer_peek(self))) {
                break;
            }
            c = lexer_consume(self);
        }

        size_t length = array_len(keyword_kind_table);
        for (u8 i = 0; i < length; i++) {
            string_token_t st = keyword_kind_table[i];
            if (strcmp(st.text, token.str.data) == 0) {
                token.kind = st.kind;
                goto fn_next_exit;
            }
        }
        token.kind = TOK_IDENT;
        goto fn_next_exit;
    }

    // integer/float
    if (isdigit(c)) {
        u8 decimal_count = 0;
        while (isdigit(c) || c == '.') {
            if (c == '.') {
                decimal_count++;
            }

            if (decimal_count > 1) {
                lexer_error(self, "invalid integer format");
            }

            string_push_char(arena, &token.str, c);
            char c_next = lexer_peek(self);
            if ((c_next != '.') && (!isdigit(c_next))) {
                break;
            }

            c = lexer_consume(self);
        }

        if (decimal_count) {
            token.kind = TOK_FLOAT;
        } else {
            token.kind = TOK_INTEGER;
        }

        goto fn_next_exit;
    }

fn_next_exit:
    if (token.kind == TOK_COMMENT) {
        lexer_skip_comment(self);
    }

    // string_deinit(&token.str);
    // string_move(&token.str, &token.str);

    if (token.kind == TOK_INVALID) {
        lexer_error(self, "invalid token");
    }

    char* tok_str = token_string(&local_arena, &token);
    log("found %s\n", tok_str);
    // xfree(tok_str);

    // string_deinit(&op_string);

    arena_deinit(&local_arena);
    log("exiting lexer_next\n");
    // arena_free(&local_arena);
    return token;
}

// i64 lexer_meta_str(lexer_t* self, char* meta_str) {
//     return sprintf(meta_str,
//                    "lexer_t{token=%s, loc=%zu, length=%zu, line=%zu, col=%zu}",
//                    self->token_string->data, self->meta.loc, self->meta.length,
//                    self->meta.line + 1, self->meta.col + 1);
// }

char lexer_peek(lexer_t* self) { return self->data[self->meta.loc]; }

char lexer_peek_next(lexer_t* self) {
    if (self->meta.loc == self->meta.length - 1) {
        return -1;
    }

    return self->data[self->meta.loc + 1];
}

char lexer_consume(lexer_t* self) {
    if (self->meta.loc == self->meta.length - 1) {
        return -1;
    }

    char c = self->data[self->meta.loc++];

    self->meta.last_col = self->meta.col;

    if (c == '\n') {
        self->meta.line++;
        self->meta.col = 0;
    } else {
        self->meta.col++;
    }

    return c;
}
