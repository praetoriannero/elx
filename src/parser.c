#include "parser.h"
#include "lexer.h"
#include "logger.h"
#include "panic.h"
#include "str_utils.h"
#include "todo.h"
#include "token.h"
#include "token_kind.h"
#include "vector.h"
#include "xalloc.h"
#include <stdio.h>

token_t parser_expect(token_kind_t expected, token_t actual) {
    if (expected != actual.kind) {
        panic("expected token kind %s but found %s, \"%s\"",
              token_kind_str(expected), token_kind_str(actual.kind),
              actual.str.data);
    }

    return actual;
}

void parser_expect_and_free(token_kind_t expected, token_t actual) {
    token_t token = parser_expect(expected, actual);
    token_deinit(&token);
}

vector_t visit_module_inner(parser_t* self) {
    token_t token = {0};
    vector_t symbol_vec = {0};
    char* tok_str;
    // vector_init(&symbol_vec, sizeof(symbol_t), 8);

    while (true) {
        token_deinit(&token);
        token = lexer_next(&self->lexer);
        switch (token.kind) {
        case TOK_COMMENT:
            continue;
        case TOK_KW_STRUCT:
            self->visit_struct(self);
            continue;
        case TOK_KW_MODULE:
            self->visit_module(self);
            continue;
        case TOK_KW_LET:
            self->visit_global(self);
            continue;
        case TOK_KW_ENUM:
            self->visit_enum(self);
            continue;
        case TOK_KW_USE:
            self->visit_import(self);
            continue;
        case TOK_EOF:
            break;
        default:
            tok_str = token_string(&token);
            snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE, "unexpected token encountered: %s\n", tok_str);
            xfree(tok_str);
            token_deinit(&token);
            xfree(token.str.data);
            panic(PANIC_MSG_BUFF);
            // panic("unexpected token encountered: %s\n", token_string(&token));
        }
    }

    token_deinit(&token);
    // token = lexer_next(&self->lexer);

    return symbol_vec;
}

ast_t parser_parse(parser_t* self) {
    ast_t ast = {0};
    visit_module_inner(self);
    return ast;
}

symbol_t* parser_visit_struct(parser_t* self) {
    log("visiting struct\n");
    symbol_t* symbol = NULL;  // xnew(symbol_t);
    struct_t struct_ = {0};

    token_t feed = parser_expect(TOK_IDENT, lexer_next(&self->lexer));
    // struct_.ident.name = strdup(feed.str.data);
    token_deinit(&feed);

    parser_expect_and_free(TOK_LBRACE, lexer_next(&self->lexer));

    feed = lexer_next(&self->lexer);
    log("%s\n", feed.str.data);

    while (feed.kind != TOK_RBRACE) {
        struct_field_t field = {0};
        parser_expect(TOK_IDENT, feed);
        // field.name = strdup(feed.str.data);
        token_deinit(&feed);

        parser_expect_and_free(TOK_COLON, lexer_next(&self->lexer));

        feed = parser_expect(TOK_IDENT, lexer_next(&self->lexer));
        // field.type.name = strdup(feed.str.data);
        token_deinit(&feed);

        feed = lexer_next(&self->lexer);

        vector_push(&struct_.fields, &field);

        if (feed.kind == TOK_COMMA) {
            token_deinit(&feed);
            feed = lexer_next(&self->lexer);
        } else {
            token_deinit(&feed);
            panic("expected either ',' or '}' but found %s", feed.str.data);
        }
    }

    token_deinit(&feed);

    // symbol->kind = SYMBOL_STRUCT;
    // symbol->variant.struct_case = struct_;
    return symbol;
}

symbol_t* parser_visit_module(parser_t* self) {
    symbol_t* symbol = NULL; // xnew(symbol_t);
    module_t module = {0};

    token_t feed = parser_expect(TOK_IDENT, lexer_next(&self->lexer));
    // module.ident.name = strdup(feed.str.data);
    token_deinit(&feed);

    parser_expect_and_free(TOK_LBRACE, lexer_next(&self->lexer));

    module.symbol_vec = visit_module_inner(self);

    parser_expect_and_free(TOK_RBRACE, lexer_next(&self->lexer));

    // symbol->kind = SYMBOL_MODULE;
    // symbol->variant.module_case = module;

    return symbol;
}

symbol_t* parser_visit_func(parser_t* self) { todo(); }

symbol_t* parser_visit_global(parser_t* self) {
    symbol_t* symbol = NULL; // xnew(symbol_t);
    global_t global = {0};

    token_t feed = parser_expect(TOK_IDENT, lexer_next(&self->lexer));
    // global.name = strdup(feed.str.data);
    token_deinit(&feed);

    feed = lexer_next(&self->lexer);
    if (feed.kind == TOK_COLON) {
        token_deinit(&feed);
        feed = parser_expect(TOK_IDENT, lexer_next(&self->lexer));
        // global.type.name = strdup(feed.str.data);
    }

    // global.mut = mut;

    // symbol->kind = SYMBOL_GLOBAL;
    // symbol->variant.global_case = global;

    token_deinit(&feed);
    return symbol;
}

symbol_t* parser_visit_enum(parser_t* self) {
    symbol_t* symbol = xnew(symbol_t);
    enum_t enm = {0};

    token_t enum_name = parser_expect(TOK_IDENT, lexer_next(&self->lexer));
    enm.name = strdup(enum_name.str.data);

    todo();
}

symbol_t* parser_visit_import(parser_t* self) { todo(); }

parser_t* parser_new(lexer_t lexer) {
    parser_t* result = xmalloc(sizeof(parser_t));
    *result = (parser_t){
        .lexer = lexer,
        .parse = &parser_parse,
        .visit_struct = &parser_visit_struct,
        .visit_module = &parser_visit_module,
        .visit_func = &parser_visit_func,
        .visit_global = &parser_visit_global,
        .visit_enum = &parser_visit_enum,
        .visit_import = &parser_visit_import,
    };

    return result;
}

void parser_init(parser_t* self, lexer_t lexer) { todo(); }

void parser_deinit(parser_t* self, lexer_t lexer) { todo(); }
