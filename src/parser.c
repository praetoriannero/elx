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

symbol_t visit_struct(arena_t* arena, parser_t* self);

symbol_t visit_module(arena_t* arena, parser_t* self);

symbol_t visit_global(arena_t* arena, parser_t* self);

symbol_t visit_enum(arena_t* arena, parser_t* self);

symbol_t visit_import(arena_t* arena, parser_t* self);

symbol_t visit_func(arena_t* arena, parser_t* self);

stmt_t visit_expr_stmt(arena_t* arena, parser_t* self);

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

    token_t feed = {0};

    arena_t local_arena = {0};
    arena_init(&local_arena);

    expr_t expr = (expr_t){0};

    feed = lexer_next(arena, &self->lexer);
    while (feed.kind != TOK_SEMICOLON) {
        vector_push(arena, &expr.tokens, &feed);
        feed = lexer_next(arena, &self->lexer);
        if (feed.kind == TOK_EOF) {
            panic("unexpected end of expression\n");
        }
    }

    return expr;
}

stmt_t visit_expr_stmt(arena_t* arena, parser_t* self) {
    todo();
    stmt_t stmt = {0};
    visit_expr(arena, self);

    return stmt;
}

stmt_t visit_for_stmt(arena_t* arena, parser_t* self) {
    todo();
}

stmt_t visit_if_stmt(arena_t* arena, parser_t* self) {
    arena_t local_arena = {0};
    arena_init(&local_arena);

    stmt_t stmt = {0};
    if_stmt_t if_stmt = {0};

    token_t feed = lexer_next(arena, &self->lexer);

    stmt.kind = STMT_KIND_IF;
    stmt.variant.if_stmt = if_stmt;

    todo();
    return stmt;
}

stmt_t visit_while_stmt(arena_t* arena, parser_t* self) {
    todo();
}

stmt_t visit_return_stmt(arena_t* arena, parser_t* self) {
    todo();
}

stmt_t visit_continue_stmt(arena_t* arena, parser_t* self) {
    todo();
}

stmt_t visit_let_stmt(arena_t* arena, parser_t* self) {
    todo();
}

body_t visit_body(arena_t* arena, parser_t* self) {
    body_t body = {0};
    vector_init(arena, &body.stmts, sizeof(stmt_t), 4);

    stmt_t stmt = {0};

    arena_t local_arena = {0};
    arena_init(&local_arena);

    char* tok_str = NULL;

    while (true) {
        token_t feed = lexer_next(&local_arena, &self->lexer);
        switch (feed.kind) {
            case TOK_IDENT:
                stmt = visit_expr_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_FOR:
                stmt = visit_for_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_IF:
                stmt = visit_if_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_WHILE:
                stmt = visit_while_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_RETURN:
                stmt = visit_return_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_CONTINUE:
                stmt = visit_continue_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_LET:
                stmt = visit_let_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_RBRACE:
                goto visit_body_exit;
            default:
                tok_str = token_string(&local_arena, &feed);
                snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE,
                         "unexpected token encountered: %s\n", tok_str);
                arena_deinit(&local_arena);
                panic(PANIC_MSG_BUFF);
        }

    }

visit_body_exit:
    arena_deinit(&local_arena);
    return body;
}

vector_t visit_module_inner(arena_t* arena, parser_t* self, u8 is_source) {
    log("entering visit_module_inner\n");

    token_t token = {0};
    char* tok_str;

    arena_t local_arena = {0};
    arena_init(&local_arena);

    symbol_t symbol = {0};

    vector_t symbol_vec = {0};
    vector_init(arena, &symbol_vec, sizeof(symbol_t), 8);

    while (true) {
        log("module inner loop start\n");
        arena_deinit(&local_arena);
        token = lexer_next(&local_arena, &self->lexer);
        switch (token.kind) {
        case TOK_COMMENT:
            continue;
        case TOK_KW_STRUCT:
            symbol = visit_struct(arena, self);
            vector_push(arena, &symbol_vec, &symbol);
            continue;
        case TOK_KW_MODULE:
            symbol = visit_module(arena, self);
            vector_push(arena, &symbol_vec, &symbol);
            continue;
        case TOK_KW_LET:
            symbol = visit_global(arena, self);
            vector_push(arena, &symbol_vec, &symbol);
            continue;
        case TOK_KW_ENUM:
            symbol = visit_enum(arena, self);
            vector_push(arena, &symbol_vec, &symbol);
            continue;
        case TOK_KW_USE:
            symbol = visit_import(arena, self);
            vector_push(arena, &symbol_vec, &symbol);
            continue;
        case TOK_KW_FN:
            symbol = visit_func(arena, self);
            vector_push(arena, &symbol_vec, &symbol);
            continue;
        case TOK_EOF:
            goto module_exit;
        case TOK_RBRACE:
            if (is_source) {
                goto module_error;
            }
            goto module_exit;
        default:

module_error:
            tok_str = token_string(&local_arena, &token);
            snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE,
                     "unexpected token encountered: %s\n", tok_str);
            arena_deinit(&local_arena);
            panic(PANIC_MSG_BUFF);
        }
        break;
    }

module_exit:

    arena_deinit(&local_arena);
    log("leaving visit_module_inner\n");
    return symbol_vec;
}

ast_t parser_parse(arena_t* arena, parser_t* self) {
    log("entering parser_parse\n");
    ast_t ast = {0};
    ast.module_vec = visit_module_inner(arena, self, 1);
    return ast;
}

void print_struct(struct_t* struct_) {
    printf("Struct{\n");
    printf("    ident=%s,\n", struct_->ident.name);
    for (u32 i = 0; i < struct_->field_vec.size; i++) {
        struct_field_t* field = vector_get(&struct_->field_vec, i);
        xnotnull(field);
        printf("    Field{ident=%s, type=%s},\n", field->name,
               field->type.ident.name);
    }
    printf("}\n");
}

symbol_t visit_struct(arena_t* arena, parser_t* self) {
    log("visiting struct\n");
    arena_t local_arena = {0};
    arena_init(&local_arena);

    symbol_t symbol = (symbol_t){0};

    struct_t struct_ = (struct_t){0};
    vector_init(arena, &struct_.field_vec, sizeof(struct_field_t), 2);

    // struct name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    struct_.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(arena, &self->lexer);

    // struct fields
    while (feed.kind != TOK_RBRACE) {
        struct_field_t field = {0};

        parser_expect(TOK_IDENT, feed);
        field.name = strdup(arena, feed.str.data);

        parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));

        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        field.type.ident.name = strdup(arena, feed.str.data);

        vector_push(arena, &struct_.field_vec, &field);

        feed = lexer_next(&local_arena, &self->lexer);

        if (feed.kind == TOK_COMMA) {
            feed = lexer_next(arena, &self->lexer);
        } else {
            panic("expected ',' but found %s", feed.str.data);
        }
    }

    print_struct(&struct_);

    symbol.kind = SYMBOL_KIND_STRUCT;
    symbol.variant.struct_case = struct_;

    arena_deinit(&local_arena);

    log("leaving struct\n");
    return symbol;
}

symbol_t visit_module(arena_t* arena, parser_t* self) {
    log("visiting module\n");
    arena_t local_arena = {0};
    symbol_t symbol = (symbol_t){0};

    // module_t* module = arena_alloc(arena, sizeof(module_t));
    module_t module = (module_t){0};
    // module.symbol_vec = arena_alloc(arena, sizeof(vector_t));
    vector_init(arena, &module.symbol_vec, sizeof(symbol_t), 8);

    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    module.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    module.symbol_vec = visit_module_inner(arena, self, 0);

    // parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    symbol.kind = SYMBOL_KIND_MODULE;
    symbol.variant.module_case = module;

    arena_deinit(&local_arena);
    log("leaving module\n");
    return symbol;
}

void print_func(func_t* func) {
    printf("Func{\n    ident=%s,\n    args={", func->ident.name);
    if (func->arg_vec.size) {
        printf("\n");
        for (usize idx = 0; idx < func->arg_vec.size; idx++) {
            func_arg_t* func_arg = vector_get(&func->arg_vec, idx);
            printf("        %s: %s,\n", func_arg->name, func_arg->type.ident.name);
        }
        printf("    }\n");
    } else {
        printf("},\n");
    }
    printf("    return_type=%s,\n    body={\n", func->ret_type.ident.name);
    printf("    },\n}\n");
}

symbol_t visit_func(arena_t* arena, parser_t* self) {
    log("visiting func\n");
    xnotnull(arena);
    xnotnull(self);
    arena_t local_arena = {0};
    symbol_t symbol = (symbol_t){0};
    func_t func = (func_t){0};
    vector_init(arena, &func.arg_vec, sizeof(func_arg_t), 4);

    // function name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    func.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LPAREN, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(arena, &self->lexer);

    // function arguments
    while (feed.kind != TOK_RPAREN) {
        func_arg_t arg = {0};
        arg.name = strdup(arena, feed.str.data);
        parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));
        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        arg.type.ident.name = strdup(arena, feed.str.data);
        vector_push(arena, &func.arg_vec, &arg);

        feed = lexer_next(&local_arena, &self->lexer);
        if (feed.kind == TOK_COMMA) {
            feed = lexer_next(arena, &self->lexer);
        }
    }

    parser_expect(TOK_RPAREN, feed);

    parser_expect(TOK_ARROW, lexer_next(&local_arena, &self->lexer));
    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    func.ret_type.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    func.body = visit_body(arena, self);

    symbol.kind = SYMBOL_KIND_FUNC;
    symbol.variant.func_case = func;

    arena_deinit(&local_arena);
    print_func(&func);
    log("leaving func\n");
    return symbol;
}

symbol_t visit_global(arena_t* arena, parser_t* self) {
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

    log("visiting global\n");

    token_t feed = (token_t){0};

    arena_t local_arena = {0};
    arena_init(&local_arena);

    symbol_t symbol = (symbol_t){0};

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

    symbol.kind = SYMBOL_KIND_GLOBAL;
    symbol.variant.global_case = global;

    arena_deinit(&local_arena);
    log("leaving global\n");
    return symbol;
}

symbol_t visit_enum(arena_t* arena, parser_t* self) {
    log("visiting enum\n");
    xnotnull(arena);
    xnotnull(self);

    arena_t local_arena = {0};
    arena_init(&local_arena);

    symbol_t symbol = (symbol_t){0};
    enum_t enum_ = {0};

    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    enum_.ident.name = strdup(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(&local_arena, &self->lexer);
    while (feed.kind != TOK_RBRACE) {
        if (feed.kind == TOK_EOF) {
            panic("invalid syntax\n");
        }

        enum_kind_t* enum_kind = arena_alloc(arena, sizeof(enum_kind_t));

        parser_expect(TOK_IDENT, feed);
        enum_kind->name = strdup(arena, feed.str.data);

        feed = lexer_next(&local_arena, &self->lexer);
        if (feed.kind == TOK_COMMA) {
            enum_kind->variant = ENUM_VARIANT_NOM;
            feed = lexer_next(&local_arena, &self->lexer);
            continue;
        }

        if (feed.kind == TOK_LPAREN) {
            enum_kind->variant = ENUM_VARIANT_ALG;
            feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
            enum_kind->type.ident.name = strdup(arena, feed.str.data);
            parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));
            parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
        }
    }

    parser_expect(TOK_RBRACE, feed);

    symbol.kind = SYMBOL_KIND_ENUM;
    symbol.variant.enum_case = enum_;

    arena_deinit(&local_arena);
    log("leaving enum\n");
    return symbol;
}

symbol_t visit_import(arena_t* arena, parser_t* self) {
    log("visiting import\n");
    todo();
    xnotnull(arena);
    xnotnull(self);
    symbol_t symbol = (symbol_t){0};

    log("leaving import\n");
    return symbol;
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
