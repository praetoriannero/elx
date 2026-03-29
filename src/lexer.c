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

void lexer_error(Lexer* self, char* msg) {
    panic("lexing error: %s at location %d:%d in file FILE\n", msg, self->context.line, self->context.col);
}

void lexer_init(Lexer* self, char* data) {
    xnotnull(self);

    self->data = data;
    self->context.loc = 0;
    self->context.col = 0;
    self->context.last_col = 0;
    self->context.length = strlen(data);
    self->context.line = 0;
}

static const char* whitespace = " \t\n\r";

static void lexer_skip_whitespace(Lexer* self) {
    char c = lexer_peek_char(self);
    while (strchr(whitespace, c)) {
        c = lexer_consume(self);
        if (c == -1) {
            break;
        }
        c = lexer_peek_char(self);
    }
}

static void lexer_skip_comment(Lexer* self) {
    char c = lexer_peek_char(self);
    while (c != '\n') {
        c = lexer_consume(self);
        if (c == -1) {
            break;
        }
        c = lexer_peek_char(self);
    }
}

Token _lexer_advance(Arena* arena, Lexer* self) {
    Arena local_arena;
    arena_init(&local_arena);

    char c;

    Token token;
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
        TokenKind kind_next = single_char_token[(u8)c_next];

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
            string_Token st = keyword_kind_table[i];
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
        // LexerContext context = self->context; // used if we need to roll-back during range operator lexing
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

Token lexer_next(Arena* arena, Lexer* self) {
    Token token = _lexer_advance(arena, self);
    log("%s\n", token_string(arena, &token));
    return token;
}

Token lexer_peek(Arena* arena, Lexer* self) {
    LexerContext context = self->context;
    Token token = _lexer_advance(arena, self);
    self->context = context;
    return token;
}

i64 lexer_scan(Lexer* self, TokenKind key) {
    LexerContext start_context = self->context;
    Arena local_arena = {};
    i64 pos = -1;

    arena_init(&local_arena);

    while (true) {
        LexerContext cursor = self->context;
        Token token = lexer_next(&local_arena, self);
        if (token.kind == key) {
            pos = (i64)cursor.loc;
            break;
        } else if (token.kind == TOK_EOF) {
            break;
        }
    }

    arena_deinit(&local_arena);
    self->context = start_context;

    return pos;
}

char lexer_peek_char(Lexer* self) { return self->data[self->context.loc]; }

char lexer_consume(Lexer* self) {
    if (self->context.loc == self->context.length - 1) {
        return -1;
    }

    char c = self->data[self->context.loc++];

    self->context.last_col = self->context.col;

    if (c == '\n') {
        self->context.line++;
        self->context.col = 0;
    } else {
        self->context.col++;
    }

    return c;
}
