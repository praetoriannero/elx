#include <assert.h>
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

ExprPrecedence postfix_precedence(TokenKind kind) {
    switch (kind) {
        case TOK_LBRACE:
        case TOK_LPAREN:
        case TOK_DECIMAL:
        case TOK_LBRACK:
            return EXPR_PREC_POSTFIX;
        default:
            return 0;
    }
}

ExprPrecedence infix_precedence(TokenKind kind) {
    switch (kind) {
        case TOK_LOR:
            return EXPR_PREC_LOR;

        case TOK_LAND:
            return EXPR_PREC_LAND;

        case TOK_EQEQ:
        case TOK_NOTEQ:
        case TOK_LT:
        case TOK_LTE:
        case TOK_GT:
        case TOK_GTE:
            return EXPR_PREC_COMP;

        case TOK_PIPE:
            return EXPR_PREC_LOR;

        case TOK_XOR:
            return EXPR_PREC_BXOR;

        case TOK_AMPER:
            return EXPR_PREC_BAND;

        case TOK_SHL:
        case TOK_SHR:
            return EXPR_PREC_SHIFT;

        case TOK_PLUS:
        case TOK_MINUS:
            return EXPR_PREC_ADDSUB;

        case TOK_STAR:
        case TOK_FSLASH:
            return EXPR_PREC_MULDIV;

        case TOK_EQ:
        case TOK_PLUSEQ:
        case TOK_MINEQ:
        case TOK_TIMESEQ:
        case TOK_DIVEQ:
        case TOK_MODEQ:
        case TOK_XOREQ:
        case TOK_ANDEQ:
        case TOK_OREQ:
        case TOK_SHLEQ:
        case TOK_SHREQ:
            return EXPR_PREC_ASSIGN;

        case TOK_RANGE:
        case TOK_RANGEINCL:
            return EXPR_PREC_RANGE;

        case TOK_PATH:
            return EXPR_PREC_UNAMBIGUOUS;

        default:
            return 0;
    }
}

Token parser_expect(TokenKind expected, Token actual) {
    if (expected != actual.kind) {
        panic("expected token kind %s but found %s, \"%s\"", token_kind_str(expected), token_kind_str(actual.kind),
              actual.str.data);
    }

    return actual;
}

Expr* parse_expr_prec(Arena* arena, Parser* self, TokenKind stop_token, ExprPrecedence min_prec) {
    log("stop token %s\n", token_kind_str(stop_token));
    Arena local_arena = {};
    arena_init(&local_arena);

    Expr* lhs = arena_alloc(arena, sizeof(Expr));
    Expr* prev_lhs;
    Token left_tok = lexer_peek(arena, &self->lexer);

    if (left_tok.kind == stop_token) {
        // empty expression
        log("lhs end of expr\n");
        lhs->expr_kind = EXPR_KIND_EMPTY;
        return lhs;
    }

    left_tok = lexer_next(arena, &self->lexer);

    if (left_tok.kind == TOK_IDENT) {
        // identifier
        log("identifier expr\n");
        lhs->expr_kind = EXPR_KIND_IDENT;
        lhs->ident_expr = strdup2(arena, left_tok.str.data);

        // todo: missing byte and char literal support in AST
    } else if (left_tok.kind == TOK_FLOAT) {
        // float literal
        log("float expr\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_FLOAT;
        lhs->literal_expr.float_ = strdup2(arena, left_tok.str.data);

    } else if (left_tok.kind == TOK_INTEGER) {
        // integer literal
        log("integer expr\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_INTEGER;
        lhs->literal_expr.integer = strdup2(arena, left_tok.str.data);

    } else if (left_tok.kind == TOK_STRING) {
        // string literal
        log("string expr\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_STRING;
        lhs->literal_expr.string = strdup2(arena, left_tok.str.data);

    } else if ((left_tok.kind == TOK_KW_FALSE) || (left_tok.kind == TOK_KW_TRUE)) {
        log("bool expr\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_BOOL;
        lhs->literal_expr.bool_ = strdup2(arena, left_tok.str.data);

    } else if (left_tok.kind == TOK_LBRACK) {
        // array implicit literal, ex: [u32; 10]
        u64 comma_count = count_csv(self->lexer.data + self->lexer.context.loc, '(');
        if (!comma_count) {
            log("array implicit expr\n");
            lhs->expr_kind = EXPR_KIND_ARRAY_IMPLICIT;

            Expr* element_type = parse_expr_prec(arena, self, TOK_SEMICOLON, 0);
            parser_expect(TOK_SEMICOLON, lexer_next(&local_arena, &self->lexer));
            Expr* element_count = parse_expr_prec(arena, self, TOK_RBRACK, 0);
            parser_expect(TOK_RBRACK, lexer_next(&local_arena, &self->lexer));

            lhs->array_implicit_expr.kind = element_type;
            lhs->array_implicit_expr.size = element_count;

        } else {
            // array explicit literal, ex: [1, 2, 3]
            log("array explicit expr\n");
            lhs->expr_kind = EXPR_KIND_ARRAY_EXPLICIT;
            for (u64 i = 0; i < comma_count; i++) {
                Expr* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                vector_push(arena, &lhs->array_explicit_expr.arg_vec, arg_expr);
            }

            if (lexer_peek(arena, &self->lexer).kind != TOK_RBRACK) {
                Expr* arg_expr = parse_expr_prec(arena, self, TOK_RBRACK, 0);
                parser_expect(TOK_RBRACK, lexer_next(&local_arena, &self->lexer));
                vector_push(arena, &lhs->array_explicit_expr.arg_vec, arg_expr);
            }
        }

    } else if (left_tok.kind == TOK_LPAREN) {
        // group expression
        // todo: support tuples, ex: (1, 2, 3)
        log("group expr\n");
        lhs = parse_expr_prec(arena, self, TOK_RPAREN, 0);
        parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));

    } else if (is_valid_op(left_tok.kind)) {
        // unary expression
        log("unary expr\n");
        lhs->expr_kind = EXPR_KIND_UNARY;
        lhs->unary_expr.op = *token_copy(arena, &left_tok);
        lhs->unary_expr.inner = parse_expr_prec(arena, self, stop_token, EXPR_PREC_PREFIX - 1);
        // parser_expect(stop_token, lexer_next(&local_arena, &self->lexer));
    }

    while (true) {
        // log("checking rhs\n");
        Token operator_tok = lexer_peek(arena, &self->lexer);
        if (operator_tok.kind == stop_token) {
            log("rhs end of expr\n");
            // end of expression
            break;
        }

        if (!is_valid_op(operator_tok.kind)) {
            // invalid operator
            panic("invalid token encountered during expression parse: '%s'", token_string(arena, &operator_tok));
        }

        ExprPrecedence postfix_prec = postfix_precedence(operator_tok.kind);
        if (postfix_prec) {
            if (postfix_prec < min_prec) {
                log("postfix break\n");
                break;
            }

            operator_tok = lexer_next(arena, &self->lexer);

            if (operator_tok.kind == TOK_LBRACE) {
                // struct init expression
                log("struct init expr\n");
                prev_lhs = lhs;

                lhs = arena_alloc(arena, sizeof(Expr));
                lhs->expr_kind = EXPR_KIND_STRUCT;

                lhs->struct_init_expr.object = prev_lhs;
                vector_init(arena, &lhs->struct_init_expr.arg_vec, sizeof(Expr), 1);

                u64 comma_count = count_csv(self->lexer.data + self->lexer.context.loc, '{');
                for (u64 i = 0; i < comma_count; i++) {
                    Expr* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                    parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                    vector_push(arena, &lhs->struct_init_expr.arg_vec, arg_expr);
                }

                if (lexer_peek(arena, &self->lexer).kind != TOK_RBRACE) {
                    Expr* arg_expr = parse_expr_prec(arena, self, TOK_RBRACE, 0);
                    vector_push(arena, &lhs->struct_init_expr.arg_vec, arg_expr);
                }

                parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));
                continue;

            } else if (operator_tok.kind == TOK_LPAREN) {
                // call or group expression
                if (lhs->expr_kind == EXPR_KIND_BINARY) {
                    // group expression
                    panic("unhandled binary expression preceding group - edge case\n");
                } else {
                    // call expression
                    log("call expr\n");
                    prev_lhs = lhs;

                    lhs = arena_alloc(arena, sizeof(Expr));
                    lhs->expr_kind = EXPR_KIND_CALL;
                    lhs->call_expr.object = prev_lhs;
                    vector_init(arena, &lhs->call_expr.arg_vec, sizeof(Expr), 1);

                    u64 comma_count = count_csv(self->lexer.data + self->lexer.context.loc, '(');
                    for (u64 i = 0; i < comma_count; i++) {
                        Expr* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                        parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->call_expr.arg_vec, arg_expr);
                    }

                    if (lexer_peek(arena, &self->lexer).kind != TOK_RPAREN) {
                        Expr* arg_expr = parse_expr_prec(arena, self, TOK_RPAREN, 0);
                        vector_push(arena, &lhs->call_expr.arg_vec, arg_expr);
                    }

                    parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));
                    continue;
                }

            } else if (operator_tok.kind == TOK_DECIMAL) {
                // field-type expression
                operator_tok = lexer_next(arena, &self->lexer); // field
                if (TOK_IDENT != operator_tok.kind) {
                    panic("expected field identifier after token '.' but found '%s'", operator_tok.str.data);
                }

                Token lparen = lexer_peek(arena, &self->lexer); // possible method call
                if (lparen.kind == TOK_LPAREN) {
                    lparen = lexer_next(arena, &self->lexer); // method call
                    // method call expression
                    log("method call expr\n");
                    prev_lhs = lhs;

                    lhs = arena_alloc(arena, sizeof(Expr));
                    vector_init(arena, &lhs->method_call_expr.arg_vec, sizeof(Expr), 1);

                    lhs->expr_kind = EXPR_KIND_METHOD_CALL;
                    lhs->method_call_expr.object = prev_lhs;
                    lhs->method_call_expr.method = strdup2(arena, operator_tok.str.data);

                    u64 comma_count = count_csv(self->lexer.data + self->lexer.context.loc, '(');
                    for (u64 i = 0; i < comma_count; i++) {
                        Expr* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                        parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->method_call_expr.arg_vec, arg_expr);
                    }

                    if (lexer_peek(arena, &self->lexer).kind != TOK_RPAREN) {
                        Expr* arg_expr = parse_expr_prec(arena, self, TOK_RPAREN, 0);
                        parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->method_call_expr.arg_vec, arg_expr);
                    }

                    continue;
                }

                prev_lhs = lhs;

                lhs = arena_alloc(arena, sizeof(Expr));
                lhs->expr_kind = EXPR_KIND_FIELD;
                lhs->field_expr.object = prev_lhs;
                lhs->field_expr.name = strdup2(arena, operator_tok.str.data);
                continue;

            } else if (operator_tok.kind == TOK_LBRACK) {
                // array index expression
                log("array index expr\n");
                prev_lhs = lhs;
                lhs = arena_alloc(arena, sizeof(Expr));
                lhs->expr_kind = EXPR_KIND_ARRAY_INDEX;
                lhs->array_index_expr.object = prev_lhs;
                lhs->array_index_expr.index = parse_expr_prec(arena, self, TOK_RBRACK, EXPR_PREC_POSTFIX - 1);
                parser_expect(TOK_RBRACK, lexer_next(&local_arena, &self->lexer));
                continue;
            }
        }

        ExprPrecedence infix_prec = infix_precedence(operator_tok.kind);
        if (infix_prec) {
            if (infix_prec < min_prec) {
                break;
            }

            operator_tok = lexer_next(arena, &self->lexer);

            if (operator_tok.kind == TOK_PATH) {
                log("path expr\n");

                prev_lhs = lhs;
                lhs = arena_alloc(arena, sizeof(Expr));
                lhs->expr_kind = EXPR_KIND_PATH;
                assert(prev_lhs->expr_kind == EXPR_KIND_IDENT);
                lhs->path_expr.stem = strdup2(arena, prev_lhs->ident_expr);
                lhs->path_expr.expr = parse_expr_prec(arena, self, stop_token, infix_prec - 1);
                continue;

            } else {
                log("binary expr %s\n", token_kind_str(operator_tok.kind));

                prev_lhs = lhs;
                lhs = arena_alloc(arena, sizeof(Expr));
                lhs->expr_kind = EXPR_KIND_BINARY;
                lhs->binary_expr.lhs = prev_lhs;
                lhs->binary_expr.op = *token_copy(arena, &operator_tok);
                lhs->binary_expr.rhs = parse_expr_prec(arena, self, stop_token, infix_prec + 1);
                continue;
            }
        }

        // if (lexer_peek(&local_arena, &self->lexer).kind != stop_token) {
        //     continue;
        // }

        log("end of expr\n");
        log("next token %s\n", token_kind_str(operator_tok.kind));
        break;
    }

    arena_deinit(&local_arena);
    return lhs;
}

Expr* visit_expr(Arena* arena, Parser* self, TokenKind stop_token) {
    /*
     * Consume tokens until we reach the stop token, creating an expression tree
     */
    return parse_expr_prec(arena, self, stop_token, 0);
}

Stmt visit_break_stmt(Arena* arena, Parser* self) {
    // scope_t* scope = arena_new_scope(arena);

    parser_expect(TOK_KW_BREAK, lexer_next(arena, &self->lexer));
    Stmt stmt = {};

    stmt.kind = STMT_KIND_BREAK;

    // todo: support continue by explicitly listing loop identifier
    stmt.break_stmt.ident = "UNSUPPORTED";

    // arena_free_scope(arena, scope);
    return stmt;
}

Stmt visit_expr_stmt(Arena* arena, Parser* self) {
    Stmt stmt = {};

    Expr* expr = visit_expr(arena, self, TOK_SEMICOLON);

    // scope_t* scope = arena_new_scope(arena);
    parser_expect(TOK_SEMICOLON, lexer_next(arena, &self->lexer));
    // arena_free_scope(arena, scope);

    stmt.kind = STMT_KIND_EXPR;
    stmt.expr_stmt.expr = expr;
    return stmt;
}

Stmt visit_for_stmt(Arena* arena, Parser* self) {
    Arena local_arena = {};
    Stmt stmt = {};
    Token feed = {};

    arena_init(&local_arena);

    parser_expect(TOK_KW_FOR, lexer_next(&local_arena, &self->lexer));

    feed = parser_expect(TOK_IDENT, lexer_next(&local_arena, &self->lexer));
    stmt.kind = STMT_KIND_FOR;

    stmt.for_stmt.iterator = strdup2(arena, feed.str.data);
    parser_expect(TOK_KW_IN, lexer_next(&local_arena, &self->lexer));
    stmt.for_stmt.iterable = visit_expr(arena, self, TOK_LBRACE);
    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
    stmt.for_stmt.body = visit_body(arena, self);
    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    arena_deinit(&local_arena);
    return stmt;
}

Stmt visit_if_stmt(Arena* arena, Parser* self) {
    Arena local_arena = {};
    Stmt stmt = {};
    Token feed = {};

    arena_init(&local_arena);

    // 'if' token
    feed = parser_expect(TOK_KW_IF, lexer_next(&local_arena, &self->lexer));

    stmt.kind = STMT_KIND_IF;
    stmt.if_stmt.condition = visit_expr(arena, self, TOK_LBRACE);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    stmt.if_stmt.body = visit_body(arena, self);
    vector_init(arena, &stmt.if_stmt.elif_clause_vec, sizeof(ElifClause), 1);

    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    while (true) {
        feed = lexer_peek(&local_arena, &self->lexer);
        if (feed.kind != TOK_KW_ELSE) {
            break;
        }

        parser_expect(TOK_KW_ELSE, lexer_next(&local_arena, &self->lexer));

        feed = lexer_peek(&local_arena, &self->lexer); // maybe has 'if'
        if (feed.kind == TOK_KW_IF) {
            ElifClause elif_clause = {};
            parser_expect(TOK_KW_IF, lexer_next(&local_arena, &self->lexer));
            elif_clause.condition = visit_expr(arena, self, TOK_LBRACE);
            parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
            elif_clause.body = visit_body(arena, self);
            parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));
            vector_push(arena, &stmt.if_stmt.elif_clause_vec, &elif_clause);

        } else {
            ElseClause else_clause = {};
            parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
            else_clause.body = visit_body(arena, self);
            parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));
            stmt.if_stmt.else_clause = else_clause;
            break;
        }
    }

    arena_deinit(&local_arena);
    return stmt;
}

Stmt visit_while_stmt(Arena* arena, Parser* self) {
    Arena local_arena = {};
    Stmt stmt = {};

    arena_init(&local_arena);

    parser_expect(TOK_KW_WHILE, lexer_next(&local_arena, &self->lexer));
    stmt.kind = STMT_KIND_WHILE;
    stmt.while_stmt.condition = visit_expr(arena, self, TOK_LBRACE);
    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
    stmt.while_stmt.body = visit_body(arena, self);
    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    arena_deinit(&local_arena);
    return stmt;
}

Stmt visit_return_stmt(Arena* arena, Parser* self) {
    // log("entering return stmt\n");
    Stmt stmt = {};

    parser_expect(TOK_KW_RETURN, lexer_next(arena, &self->lexer));

    stmt.kind = STMT_KIND_RETURN;
    stmt.return_stmt.expr = visit_expr(arena, self, TOK_SEMICOLON);

    parser_expect(TOK_SEMICOLON, lexer_next(arena, &self->lexer));

    // log("exiting return stmt\n");
    return stmt;
}

Stmt visit_continue_stmt(Arena* arena, Parser* self) {
    parser_expect(TOK_KW_CONTINUE, lexer_next(arena, &self->lexer));
    Stmt stmt = {};
    stmt.kind = STMT_KIND_CONTINUE;
    // todo: support continue by explicitly listing loop identifier
    stmt.cont_stmt.ident = "UNSUPPORTED";

    return stmt;
}

Stmt visit_assign_stmt(Arena* arena, Parser* self) {
    Arena local_arena = {};
    arena_init(&local_arena);
    Stmt stmt = {};

    stmt.kind = STMT_KIND_ASSIGN;

    bool mut = false;
    Token feed = lexer_next(arena, &self->lexer);
    if (feed.kind == TOK_KW_VAR) {
        mut = true;
    } else {
        parser_expect(TOK_KW_LET, feed);
    }
    stmt.assign_stmt.mut = mut;

    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    stmt.assign_stmt.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_EQ, lexer_next(arena, &self->lexer));
    stmt.assign_stmt.expr = visit_expr(arena, self, TOK_SEMICOLON);
    parser_expect(TOK_SEMICOLON, lexer_next(&local_arena, &self->lexer));

    arena_deinit(&local_arena);
    return stmt;
}

Body visit_body(Arena* arena, Parser* self) {
    // log("entering visit_body\n");
    Body body = {};
    vector_init(arena, &body.stmts, sizeof(Stmt), 4);

    Stmt stmt = {};

    Arena local_arena = {};
    arena_init(&local_arena);

    char* tok_str = NULL;

    while (true) {
        Token feed = lexer_peek(&local_arena, &self->lexer);
        switch (feed.kind) {
            case TOK_KW_TRUE:
            case TOK_KW_FALSE:
            case TOK_INTEGER:
            case TOK_FLOAT:
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
            case TOK_KW_BREAK:
                stmt = visit_break_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_KW_VAR:
            case TOK_KW_LET:
                stmt = visit_assign_stmt(arena, self);
                vector_push(arena, &body.stmts, &stmt);
                continue;
            case TOK_RBRACE:
                break;
            default:
                tok_str = token_string(&local_arena, &feed);
                snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE, "unexpected token encountered: %s\n", tok_str);
                arena_deinit(&local_arena);
                panic(PANIC_MSG_BUFF);
        }
        break;
    }

    arena_deinit(&local_arena);
    // log("leaving visit_body\n");
    return body;
}

Vector visit_module_inner(Arena* arena, Parser* self, u8 is_source) {
    // log("entering visit_module_inner\n");

    Token token = {};
    char* tok_str = NULL;
    Arena local_arena = {};
    Symbol symbol = {};
    Vector symbol_vec = {};

    arena_init(&local_arena);
    vector_init(arena, &symbol_vec, sizeof(Symbol), 8);

    while (true) {
        // log("module inner loop start\n");
        arena_deinit(&local_arena);
        token = lexer_peek(&local_arena, &self->lexer);
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
            case TOK_KW_VAR:
                symbol = visit_global(arena, self, true);
                vector_push(arena, &symbol_vec, &symbol);
                continue;
            case TOK_KW_LET:
                symbol = visit_global(arena, self, false);
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
                snprintf(PANIC_MSG_BUFF, PANIC_MSG_SIZE, "unexpected token encountered: %s\n", tok_str);
                arena_deinit(&local_arena);
                panic(PANIC_MSG_BUFF);
        }
        break;
    }

module_exit:

    arena_deinit(&local_arena);
    // log("leaving visit_module_inner\n");
    return symbol_vec;
}

Ast parser_parse(Arena* arena, Parser* self) {
    // log("entering parser_parse\n");
    Ast ast = {};
    Module module = {.name = "ANONYMOUS", visit_module_inner(arena, self, true)};
    vector_init(arena, &ast.module_vec, sizeof(Module), 1);
    vector_push(arena, &ast.module_vec, &module);

    return ast;
}

Symbol visit_struct(Arena* arena, Parser* self) {
    // log("visiting struct\n");

    Arena local_arena = {};
    Symbol symbol = {};
    Struct struct_ = {};

    arena_init(&local_arena);
    vector_init(arena, &struct_.field_vec, sizeof(StructField), 1);
    vector_init(arena, &struct_.method_vec, sizeof(Symbol), 1);

    parser_expect(TOK_KW_STRUCT, lexer_next(&local_arena, &self->lexer));

    // struct name
    Token feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    struct_.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_peek(arena, &self->lexer);

    // struct contents
    while (feed.kind != TOK_RBRACE) {
        if (feed.kind == TOK_IDENT) { // struct field
            feed = lexer_next(&local_arena, &self->lexer);
            StructField field = {};

            field.name = strdup2(arena, feed.str.data);

            parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));

            feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
            field.type.name = strdup2(arena, feed.str.data);
            vector_push(arena, &struct_.field_vec, &field);

            parser_expect(TOK_SEMICOLON, lexer_next(&local_arena, &self->lexer));

        } else if (feed.kind == TOK_KW_FN) { // struct method
            Symbol method = visit_func(arena, self);
            vector_push(arena, &struct_.method_vec, &method);

        } else { // unexpected input
            panic("unexpected token '%s'", feed.str.data);
        }

        // advance stream
        feed = lexer_peek(arena, &self->lexer);
    }

    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    symbol.kind = SYMBOL_KIND_STRUCT;
    symbol.struct_case = struct_;

    arena_deinit(&local_arena);

    // log("leaving struct\n");
    return symbol;
}

Symbol visit_module(Arena* arena, Parser* self) {
    // log("visiting module\n");

    Arena local_arena = {};
    Symbol symbol = {};
    Module module = {};

    vector_init(arena, &module.symbol_vec, sizeof(Symbol), 8);

    parser_expect(TOK_KW_MODULE, lexer_next(arena, &self->lexer));

    // module name
    Token feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    module.name = strdup2(arena, feed.str.data);

    // module content
    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
    module.symbol_vec = visit_module_inner(arena, self, 0);
    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    symbol.kind = SYMBOL_KIND_MODULE;
    symbol.module_case = module;

    arena_deinit(&local_arena);
    // log("leaving module\n");
    return symbol;
}

Symbol visit_func(Arena* arena, Parser* self) {
    // log("visiting func\n");
    xnotnull(arena);
    xnotnull(self);

    Arena local_arena = {};
    arena_init(&local_arena);

    Symbol symbol = {};
    Func func = {};

    parser_expect(TOK_KW_FN, lexer_next(&local_arena, &self->lexer));
    vector_init(arena, &func.param_vec, sizeof(FuncArg), 1);

    // function name
    Token feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    func.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LPAREN, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(arena, &self->lexer);

    // function arguments
    while (feed.kind != TOK_RPAREN) {
        FuncArg arg = {};
        arg.name = strdup2(arena, feed.str.data);
        parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));
        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        arg.type.name = strdup2(arena, feed.str.data);
        vector_push(arena, &func.param_vec, &arg);

        feed = lexer_next(&local_arena, &self->lexer);
        if (feed.kind == TOK_COMMA) {
            feed = lexer_next(arena, &self->lexer);
        }
    }

    parser_expect(TOK_RPAREN, feed);

    parser_expect(TOK_ARROW, lexer_next(&local_arena, &self->lexer));
    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    func.ret_type.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    func.body = visit_body(arena, self);

    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    symbol.kind = SYMBOL_KIND_FUNC;
    symbol.func_case = func;

    arena_deinit(&local_arena);
    // log("leaving func\n");
    return symbol;
}

Symbol visit_global(Arena* arena, Parser* self, bool is_var) {
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

    // log("visiting global\n");

    Token feed = {};
    Arena local_arena = {};
    Symbol symbol = {};
    Global global = {};

    arena_init(&local_arena);

    feed = lexer_next(arena, &self->lexer);
    global.mut = is_var;

    // variable name
    feed = parser_expect(TOK_IDENT, feed);
    global.name = strdup2(arena, feed.str.data);

    feed = parser_expect(TOK_COLON, lexer_next(arena, &self->lexer));

    // type name
    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    global.type.name = strdup2(arena, feed.str.data);

    // globals must always be initialized
    feed = parser_expect(TOK_EQ, lexer_next(arena, &self->lexer));

    // remaining tokens become expression
    global.expr = visit_expr(arena, self, TOK_SEMICOLON);

    symbol.kind = SYMBOL_KIND_GLOBAL;
    symbol.global_case = global;

    arena_deinit(&local_arena);
    // log("leaving global\n");
    return symbol;
}

Symbol visit_enum(Arena* arena, Parser* self) {
    // log("visiting enum\n");
    xnotnull(arena);
    xnotnull(self);

    Arena local_arena = {};
    Symbol symbol = {};
    Enum enum_ = {};
    Token feed = {};

    arena_init(&local_arena);

    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    enum_.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(&local_arena, &self->lexer);
    while (feed.kind != TOK_RBRACE) {
        if (feed.kind == TOK_EOF) {
            panic("invalid syntax\n");
        }

        EnumKind* enum_kind = arena_alloc(arena, sizeof(EnumKind));

        parser_expect(TOK_IDENT, feed);
        enum_kind->name = strdup2(arena, feed.str.data);

        feed = lexer_next(&local_arena, &self->lexer);
        if (feed.kind == TOK_COMMA) {
            enum_kind->variant = ENUM_VARIANT_NOM;
            feed = lexer_next(&local_arena, &self->lexer);
            continue;
        }

        if (feed.kind == TOK_LPAREN) {
            enum_kind->variant = ENUM_VARIANT_ALG;
            feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
            enum_kind->type.name = strdup2(arena, feed.str.data);
            parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));
            parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
        }
    }

    parser_expect(TOK_RBRACE, feed);

    symbol.kind = SYMBOL_KIND_ENUM;
    symbol.enum_case = enum_;

    arena_deinit(&local_arena);
    // log("leaving enum\n");
    return symbol;
}

Symbol visit_import(Arena* arena, Parser* self) {
    todo();
    xnotnull(arena);
    xnotnull(self);
    Symbol symbol = {};

    return symbol;
}

Parser* parser_new(Arena* arena, Lexer lexer) {
    Parser* result = arena_alloc(arena, sizeof(Parser));
    parser_init(result, lexer);
    return result;
}

void parser_init(Parser* self, Lexer lexer) {
    *self = (Parser){
        .lexer = lexer,
    };
}
