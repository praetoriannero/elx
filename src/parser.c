#include <stdio.h>

#include "arena.h"
#include "lexer.h"
#include "logger.h"
#include "modprim.h"
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

symbol_t* parser_visit_func(arena_t* arena, parser_t* self);

stmt_t* parser_visit_expr_stmt(arena_t* arena, parser_t* self);

expr_t visit_expr(arena_t* arena, parser_t* self);

token_t parser_expect(token_kind_t expected, token_t actual) {
    if (expected != actual.kind) {
        panic("expected token kind %s but found %s, \"%s\"",
              token_kind_str(expected), token_kind_str(actual.kind),
              actual.str.data);
    }

    return actual;
}

expr_t visit_expr(arena_t* arena, parser_t* self) {
    /*
     * Consume tokens until we reach a semicolon
     */
    token_t* feed = arena_alloc(arena, sizeof(token_t));
    *feed = (token_t){0};

    arena_t local_arena = {0};
    arena_init(&local_arena);

    // expr_t* expr = arena_alloc(arena, sizeof(expr_t));
    expr_t expr = (expr_t){0};

    *feed = lexer_next(arena, &self->lexer);
    while (feed->kind != TOK_SEMICOLON) {
        vector_push(arena, &expr.tokens, feed);
        *feed = lexer_next(arena, &self->lexer);
        if (feed->kind == TOK_EOF) {
            panic("invalid syntax\n");
        }
    }

    return expr;
}

vector_t visit_module_inner(arena_t* arena, parser_t* self) {
    log("entering visit_module_inner\n");
    token_t token = {0};
    char* tok_str;
    arena_t local_arena = {0};
    arena_init(&local_arena);

    vector_t symbol_vec = {0};
    vector_init(arena, &symbol_vec, sizeof(symbol_t), 8);

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
        case TOK_KW_FN:
            parser_visit_func(arena, self);
            continue;
        case TOK_EOF:
            break;
        case TOK_RBRACE:
            break;
        default:
            tok_str = token_string(&local_arena, &token);
            snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE,
                     "unexpected token encountered: %s\n", tok_str);
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
    ast.module_vec = visit_module_inner(arena, self);
    return ast;
}

void print_struct(struct_t* struct_) {
    printf("Struct{\n");
    printf("    ident=%s,\n", struct_->ident.name);
    for (u32 i = 0; i < struct_->field_vec.size; i++) {
        struct_field_t* field = vector_get(&struct_->field_vec, i);
        xnotnull(field);
        printf("    Field{ident=%s, type=%s},\n", field->name,
               field->type.name);
    }
    printf("}\n");
}

symbol_t* parser_visit_struct(arena_t* arena, parser_t* self) {
    log("visiting struct\n");
    arena_t local_arena = {0};
    arena_init(&local_arena);

    symbol_t* symbol = arena_alloc(arena, sizeof(symbol_t));
    *symbol = (symbol_t){0};

    struct_t struct_ = (struct_t){0};
    vector_init(arena, &struct_.field_vec, sizeof(struct_field_t), 2);

    // struct name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    struct_.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(arena, &self->lexer);

    // struct fields
    while (feed.kind != TOK_RBRACE) {
        struct_field_t* field = arena_alloc(arena, sizeof(struct_field_t));
        *field = (struct_field_t){0};

        parser_expect(TOK_IDENT, feed);
        field->name = strdup(arena, feed.str.data);

        parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));

        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        field->type.name = strdup(arena, feed.str.data);

        vector_push(arena, &struct_.field_vec, field);

        feed = lexer_next(&local_arena, &self->lexer);

        if (feed.kind == TOK_COMMA) {
            feed = lexer_next(arena, &self->lexer);
        } else {
            panic("expected ',' but found %s", feed.str.data);
        }
    }

    print_struct(&struct_);

    symbol->kind = SYMBOL_STRUCT;
    symbol->variant.struct_case = struct_;

    arena_deinit(&local_arena);
    return symbol;
}

symbol_t* parser_visit_module(arena_t* arena, parser_t* self) {
    log("visiting module\n");
    arena_t local_arena = {0};
    symbol_t* symbol = arena_alloc(arena, sizeof(symbol_t));
    *symbol = (symbol_t){0};

    // module_t* module = arena_alloc(arena, sizeof(module_t));
    module_t module = (module_t){0};
    // module.symbol_vec = arena_alloc(arena, sizeof(vector_t));
    vector_init(arena, &module.symbol_vec, sizeof(symbol_t), 8);

    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    module.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    module.symbol_vec = visit_module_inner(arena, self);

    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    symbol->kind = SYMBOL_MODULE;
    symbol->variant.module_case = module;

    arena_deinit(&local_arena);
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

    token_t feed = (token_t){0};

    arena_t local_arena = {0};
    arena_init(&local_arena);

    symbol_t* symbol = arena_alloc(arena, sizeof(symbol_t));
    *symbol = (symbol_t){0};

    global_t global = (global_t){0};

    feed = lexer_next(arena, &self->lexer);
    if (feed.kind == TOK_KW_MUT) {
        global.mut = true;
        feed = lexer_next(arena, &self->lexer);
    }

    // variable name
    feed = parser_expect(TOK_IDENT, feed);
    global.name = strdup(arena, feed.str.data);

    feed = parser_expect(TOK_COLON, lexer_next(arena, &self->lexer));

    // type name
    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    global.type.name = strdup(arena, feed.str.data);

    feed = parser_expect(TOK_EQ, lexer_next(arena, &self->lexer));

    // remaining tokens become expression
    global.expr = visit_expr(arena, self);

    symbol->kind = SYMBOL_GLOBAL;
    symbol->variant.global_case = global;

    arena_deinit(&local_arena);
    return symbol;
}

symbol_t* parser_visit_enum(arena_t* arena, parser_t* self) {
    todo();
    xnotnull(arena);
    xnotnull(self);
    // symbol_t* symbol = xnew(symbol_t);
    // enum_t enum_ = {0};
    //
    // token_t enum_name = parser_expect(TOK_IDENT, lexer_next(arena,
    // &self->lexer)); enum_.name = strdup(enum_name.str.data);

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
