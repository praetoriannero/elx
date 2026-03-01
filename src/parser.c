#include <stdio.h>

#include "arena.h"
#include "lexer.h"
#include "logger.h"
#include "panic.h"
#include "parser.h"
#include "str_utils.h"
#include "todo.h"
#include "token.h"
#include "token_kind.h"
#include "vector.h"
#include "xalloc.h"


symbol_t* parser_visit_struct(arena_t* arena, parser_t* self);

symbol_t* parser_visit_module(arena_t* arena, parser_t* self);

symbol_t* parser_visit_global(arena_t* arena, parser_t* self);

symbol_t* parser_visit_enum(arena_t* arena, parser_t* self);

symbol_t* parser_visit_import(arena_t* arena, parser_t* self);

token_t parser_expect(token_kind_t expected, token_t actual) {
    if (expected != actual.kind) {
        panic("expected token kind %s but found %s, \"%s\"",
              token_kind_str(expected), token_kind_str(actual.kind),
              actual.str.data);
    }

    return actual;
}

vector_t visit_module_inner(arena_t* arena, parser_t* self) {
    log("entering visit_module_inner\n");
    token_t token = {0};
    vector_t symbol_vec = {0};
    char* tok_str;
    arena_t local_arena;
    arena_init(&local_arena);
    // vector_init(&symbol_vec, sizeof(symbol_t), 8);

    while (true) {
        arena_deinit(&local_arena);
        token = lexer_next(&local_arena, &self->lexer);
        switch (token.kind) {
        case TOK_COMMENT:
            continue;
        case TOK_KW_STRUCT:
            parser_visit_struct(arena, self);
            continue;
        case TOK_KW_MODULE:
            parser_visit_module(arena, self);
            continue;
        case TOK_KW_LET:
            parser_visit_global(arena, self);
            continue;
        case TOK_KW_ENUM:
            parser_visit_enum(arena, self);
            continue;
        case TOK_KW_USE:
            parser_visit_import(arena, self);
            continue;
        case TOK_EOF:
            break;
        default:
            tok_str = token_string(&local_arena, &token);
            snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE, "unexpected token encountered: %s\n", tok_str);
            arena_deinit(&local_arena);
            panic(PANIC_MSG_BUFF);
        }
    }

    arena_deinit(&local_arena);
    return symbol_vec;
}

ast_t parser_parse(arena_t* arena, parser_t* self) {
    log("entering parser_parse\n");
    ast_t ast = {0};
    visit_module_inner(arena, self);
    return ast;
}

symbol_t* parser_visit_struct(arena_t* arena, parser_t* self) {
    log("visiting struct\n");
    arena_t local_arena;
    arena_init(&local_arena);

    symbol_t* symbol = NULL;
    struct_t struct_ = {0};

    // struct name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    // struct_.ident.name = strdup(feed.str.data);
    // token_deinit(&feed);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
    log("found lbrace\n");

    feed = lexer_next(arena, &self->lexer);
    log("%s\n", feed.str.data);

    while (feed.kind != TOK_RBRACE) {
        struct_field_t* field = arena_alloc(arena, sizeof(struct_field_t));
        parser_expect(TOK_IDENT, feed);
        // field.name = strdup(feed.str.data);
        // token_deinit(&feed);

        parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));

        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        // field.type.name = strdup(feed.str.data);

        vector_push(&struct_.fields, field);

        feed = lexer_next(&local_arena, &self->lexer);

        if (feed.kind == TOK_COMMA) {
            feed = lexer_next(arena, &self->lexer);
        } else {
            panic("expected ',' but found %s", feed.str.data);
        }
    }

    arena_deinit(&local_arena);
    // symbol->kind = SYMBOL_STRUCT;
    // symbol->variant.struct_case = struct_;
    return symbol;
}

symbol_t* parser_visit_module(arena_t* arena, parser_t* self) {
    arena_t local_arena;
    symbol_t* symbol = NULL; // xnew(symbol_t);
    module_t module = {0};

    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    // module.ident.name = strdup(feed.str.data);
    // token_deinit(&feed);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    module.symbol_vec = visit_module_inner(arena, self);

    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    // symbol->kind = SYMBOL_MODULE;
    // symbol->variant.module_case = module;

    return symbol;
}

symbol_t* parser_visit_func(arena_t* arena, parser_t* self) {
    todo();
    xnotnull(arena);
    xnotnull(self);
    
    return NULL;
}

symbol_t* parser_visit_global(arena_t* arena, parser_t* self) {
    /*
     *  Parse a global identifier declaration
     *  examples:
     *      let x: u32 = 0;
     *      let mut foo: vec<std::String> = vec::new();
     *
     *  Initially, we expect an identifier or a `mut` keyword. Next, we
     *  must have a type specified for the global, and finally we need
     *  an assignment on the right hand side.
     *
     */

    symbol_t* symbol = NULL; // xnew(symbol_t);
    global_t global = {0};

    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    // global.name = strdup(feed.str.data);
    // token_deinit(&feed);

    feed = lexer_next(arena, &self->lexer);
    if (feed.kind == TOK_COLON) {
        // token_deinit(&feed);
        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        // global.type.name = strdup(feed.str.data);
    }

    // global.mut = mut;

    // symbol->kind = SYMBOL_GLOBAL;
    // symbol->variant.global_case = global;

    // token_deinit(&feed);
    return symbol;
}

symbol_t* parser_visit_enum(arena_t* arena, parser_t* self) {
    todo();
    xnotnull(arena);
    xnotnull(self);
    // symbol_t* symbol = xnew(symbol_t);
    // enum_t enum_ = {0};
    //
    // token_t enum_name = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    // enum_.name = strdup(enum_name.str.data);

    return NULL;
}

symbol_t* parser_visit_import(arena_t* arena, parser_t* self) {
    todo();
    xnotnull(arena);
    xnotnull(self);

    return NULL;
}

parser_t* parser_new(arena_t* arena, lexer_t lexer) {
    parser_t* result = arena_alloc(arena, sizeof(parser_t));
    parser_init(result, lexer);
    return result;
}

void parser_init(parser_t* self, lexer_t lexer) {
    *self = (parser_t){
        .lexer = lexer,
    };
}

// void parser_deinit(parser_t* self, lexer_t lexer) { todo(); }
