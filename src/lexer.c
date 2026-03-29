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
    panic("lexing error: %s at location %d:%d in file FILE\n", msg, self->meta.line, self->meta.col);
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
    char c = lexer_peek_char(self);
    while (strchr(whitespace, c)) {
        c = lexer_consume(self);
        if (c == -1) {
            break;
        }
        c = lexer_peek_char(self);
    }
}

static void lexer_skip_comment(lexer_t* self) {
    char c = lexer_peek_char(self);
    while (c != '\n') {
        c = lexer_consume(self);
        if (c == -1) {
            break;
        }
        c = lexer_peek_char(self);
    }
}

token_t _lexer_advance(arena_t* arena, lexer_t* self) {
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

    // string literal
    if (token.kind == TOK_DQUOTE) {
        string_push_char(arena, &token.str, c);
        while (lexer_peek_char(self) != '"') {
            if (lexer_peek_char(self) == -1) {
                token.kind = TOK_EOF;
                return token;
            }
            string_push_char(arena, &token.str, lexer_consume(self));
        }
        string_push_char(arena, &token.str, lexer_consume(self));
        token.kind = TOK_STRING;
        goto fn_next_exit;
    }

    // operator
    if (token.kind != TOK_INVALID) {
        string_push_char(arena, &token.str, c);

        const op_node_t* op_iter = op_table;
        size_t sub_table_size = 0;
        size_t table_size = array_len(op_table);
        op_node_t op_node;

        char c_next = lexer_peek_char(self);
        token_kind_t kind_next = single_char_token[(u8)c_next];

        // get first operator node
        for (size_t i = 0; i < table_size; i++) {
            op_node = op_iter[i];
            if (op_node.text == c) {
                op_iter = op_node.children;
                sub_table_size = op_node.child_count;
                break;
            }
        }

        bool found = false;
        while (op_iter && (kind_next != TOK_INVALID) && (!strchr(whitespace, c_next))) {
            for (size_t i = 0; i < sub_table_size; i++) {
                op_node = op_iter[i];
                if (op_node.text == c_next) {
                    string_push_char(arena, &token.str, lexer_consume(self));
                    op_iter = op_node.children;
                    sub_table_size = op_node.child_count;
                    token.kind = op_node.kind;
                    found = true;
                    break;
                }
            }

            if (!found) {
                break;
            }

            c_next = lexer_peek_char(self);
            kind_next = single_char_token[(u8)c_next];
        }

        goto fn_next_exit;
    }

    // identifier
    if (is_valid_ident_start(c)) {
        while (is_ident_char(c)) {
            string_push_char(arena, &token.str, c);
            if (!is_ident_char(lexer_peek_char(self))) {
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
        // lexer_meta_t meta = self->meta; // used if we need to roll-back during range operator lexing
        u8 decimal_count = 0;
        while (isdigit(c) || c == '.') {
            if (c == '.') {
                decimal_count++;
            }

            if (decimal_count > 1) {
                // todo: handle range expressions
                lexer_error(self, "invalid integer format");
            }

            string_push_char(arena, &token.str, c);
            char c_next = lexer_peek_char(self);
            if ((c_next != '.') && (!isdigit(c_next))) {
                break;
            }

            c = lexer_consume(self);
        }

        // bool has_decimal = false;
        // while (isdigit(c)) {
        //     string_push_char(arena, &token.str, c);
        //     char c_next = lexer_peek_char(self);
        //     if (!isdigit(c_next)) {
        //         break;
        //     }
        //     c = lexer_consume(self);
        // }
        //
        // if (c == '.') {
        //     has_decimal = true;
        //     if (lexer_peek_char(self) == '.') { // likely a range expression
        //
        //     }
        // }

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
        return lexer_next(arena, self);
    }

    if (token.kind == TOK_INVALID) {
        lexer_error(self, "invalid token");
    }

    arena_deinit(&local_arena);
    return token;
}

token_t lexer_next(arena_t* arena, lexer_t* self) {
    token_t token = _lexer_advance(arena, self);
    log("%s\n", token_string(arena, &token));
    return token;
}

token_t lexer_peek(arena_t* arena, lexer_t* self) {
    lexer_meta_t meta = self->meta;
    token_t token = _lexer_advance(arena, self);
    self->meta = meta;
    return token;
}

i64 lexer_scan(lexer_t* self, token_kind_t key) {
    lexer_meta_t start_meta = self->meta;
    arena_t local_arena = {};
    i64 pos = -1;

    arena_init(&local_arena);

    while (true) {
        lexer_meta_t cursor = self->meta;
        token_t token = lexer_next(&local_arena, self);
        if (token.kind == key) {
            pos = (i64)cursor.loc;
            break;
        } else if (token.kind == TOK_EOF) {
            break;
        }
    }

    arena_deinit(&local_arena);
    self->meta = start_meta;

    return pos;
}

char lexer_peek_char(lexer_t* self) { return self->data[self->meta.loc]; }

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
