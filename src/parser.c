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

void print_enum(enum_t* enum_, u64 depth);
void print_import(import_t* import, u64 depth);
void print_module(module_t* module, u64 depth);
void print_func(func_t* func, u64 depth);
void print_global(global_t* global, u64 depth);
void print_struct(struct_t* struct_, u64 depth);

const char INDENT[] = "  ";

void indent(u64 depth) {
    for (u64 i = 0; i < depth; i++) {
        printf("%s", INDENT);
    }
}

expr_precedence_t postfix_precedence(token_kind_t kind) {
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

expr_precedence_t infix_precedence(token_kind_t kind) {
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
        default:
            return EXPR_PREC_UNAMBIGUOUS;
    }
}

char* expr_kind_string(expr_kind_t kind) {
    switch (kind) {
            // EmptyExpression
        case EXPR_KIND_EMPTY:
            return "EXPR_KIND_EMPTY";

            // LiteralExpression
        case EXPR_KIND_LITERAL:
            return "EXPR_KIND_LITERAL";

            // PathExpression
        case EXPR_KIND_PATH:
            return "EXPR_KIND_PATH";

            // OperatorExpression
        case EXPR_KIND_BINARY:
            return "EXPR_KIND_BINARY";
        case EXPR_KIND_UNARY:
            return "EXPR_KIND_UNARY";

            // GroupedExpression
        case EXPR_KIND_GROUP:
            return "EXPR_KIND_GROUP";

            // ArrayExpression
        case EXPR_KIND_ARRAY_EXPLICIT:
            return "EXPR_KIND_ARRAY_EXPLICIT";
        case EXPR_KIND_ARRAY_IMPLICIT:
            return "EXPR_KIND_ARRAY_IMPLICIT";

            // IndexExpression
        case EXPR_KIND_ARRAY_INDEX:
            return "EXPR_KIND_ARRAY_INDEX";

            // TupleExpression
        case EXPR_KIND_TUPLE:
            return "EXPR_KIND_TUPLE";

            // note: this is really a field expression
            // TupleIndexingExpression
            // case EXPR_KIND_TUPLE_INDEX: return "EXPR_KIND_TUPLE_INDEX";

            // StructExpression
        case EXPR_KIND_STRUCT:
            return "EXPR_KIND_STRUCT";

            // CallExpression
        case EXPR_KIND_CALL:
            return "EXPR_KIND_CALL";

            // MethodCallExpression
        case EXPR_KIND_METHOD_CALL:
            return "EXPR_KIND_METHOD_CALL";

            // FieldExpression
        case EXPR_KIND_FIELD:
            return "EXPR_KIND_FIELD";

            // ClosureExpression
        case EXPR_KIND_CLOSURE:
            return "EXPR_KIND_CLOSURE";

            // RangeExpression
        case EXPR_KIND_RANGE:
            return "EXPR_KIND_RANGE";

            // AssignmentExpression
        case EXPR_KIND_ASSIGN:
            return "EXPR_KIND_ASSIGN";

            // future
            // AwaitExpression
            // AsyncBlockExpression
            // UnderscoreExpression

            // JumpExpression
            // case EXPR_KIND_RETURN: return "EXPR_KIND_RETURN";
            // case EXPR_KIND_BREAK: return "EXPR_KIND_BREAK";
            // case EXPR_KIND_CONTINUE: return "EXPR_KIND_CONTINUE";

        case EXPR_KIND_IDENT:
            return "EXPR_KIND_IDENT";
    }
}

token_t parser_expect(token_kind_t expected, token_t actual) {
    if (expected != actual.kind) {
        panic("expected token kind %s but found %s, \"%s\"", token_kind_str(expected), token_kind_str(actual.kind),
              actual.str.data);
    }

    return actual;
}

void print_enum(enum_t* enum_, u64 depth) { todo(); }

void print_import(import_t* import, u64 depth) { todo(); }

void print_ast(ast_t* ast) {
    u64 depth = 0;
    for (usize i = 0; i < ast->module_vec.size; i++) {
        module_t* module = vector_get(&ast->module_vec, i);
        print_module(module, depth);
    }
}

void print_body(body_t* body, u64 depth);

void print_expr(expr_t* expr, u64 depth) {
    xnotnull(expr);
    usize vec_len;

    indent(depth);
    switch (expr->expr_kind) {
        case EXPR_KIND_METHOD_CALL:
            printf("method call {\n");
            depth++;
            indent(depth);
            printf("object {\n");
            print_expr(expr->method_call_expr.object, depth + 1);
            indent(depth);
            printf("},\n");
            indent(depth);
            printf("field: %s,\n", expr->method_call_expr.method);
            vec_len = expr->method_call_expr.arg_vec.size;
            indent(depth);
            printf("args {\n");
            for (usize i = 0; i < vec_len; i++) {
                expr_t* arg_expr = vector_get(&expr->method_call_expr.arg_vec, i);
                print_expr(arg_expr, depth + 1);
            }
            indent(depth);
            printf("},\n");
            depth--;

            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_EMPTY:
            printf("<EMPTY>\n");
            break;

        case EXPR_KIND_IDENT:
            printf("ident: %s\n", expr->ident_expr);
            break;

        case EXPR_KIND_LITERAL:
            switch (expr->literal_expr.kind) {
                case LITERAL_KIND_STRING:
                    printf("string literal: '%s'\n", expr->literal_expr.string);
                    break;

                case LITERAL_KIND_FLOAT:
                    printf("float literal: '%s'\n", expr->literal_expr.float_);
                    break;

                case LITERAL_KIND_INTEGER:
                    printf("integer literal: '%s'\n", expr->literal_expr.integer);
                    break;

                default:
                    printf("<UNSUPPORTED_LITERAL>\n");
                    break;
            }
            break;

        case EXPR_KIND_ARRAY_IMPLICIT:
            printf("array implicit {\n");
            print_expr(expr->array_implicit_expr.kind, depth + 1);
            print_expr(expr->array_implicit_expr.size, depth + 1);
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_ARRAY_EXPLICIT:
            printf("array explicit {\n");
            vec_len = expr->array_explicit_expr.arg_vec.size;
            for (usize i = 0; i < vec_len; i++) {
                expr_t* arg_expr = vector_get(&expr->array_explicit_expr.arg_vec, i);
                print_expr(arg_expr, depth + 1);
            }
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_BINARY:
            printf("binary %s {\n", expr->binary_expr.op.str.data);
            print_expr(expr->binary_expr.lhs, depth + 1);
            print_expr(expr->binary_expr.rhs, depth + 1);
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_UNARY:
            printf("unary %s {\n", expr->unary_expr.op.str.data);
            print_expr(expr->unary_expr.inner, depth + 1);
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_ARRAY_INDEX:
            printf("array index {\n");
            print_expr(expr->array_index_expr.index, depth + 1);
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_STRUCT:
            printf("struct init {\n");
            depth++;
            indent(depth);
            printf("object {\n");
            print_expr(expr->struct_init_expr.object, depth + 1);
            indent(depth);
            printf("},\n");
            depth--;
            vec_len = expr->struct_init_expr.arg_vec.size;
            depth++;
            indent(depth);
            printf("args {\n");
            for (usize i = 0; i < vec_len; i++) {
                expr_t* arg_expr = vector_get(&expr->struct_init_expr.arg_vec, i);
                print_expr(arg_expr, depth + 1);
            }
            indent(depth);
            printf("},\n");
            depth--;
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_FIELD:
            printf("field access {\n");
            depth++;
            indent(depth);
            printf("object {\n");
            print_expr(expr->field_expr.object, depth + 1);
            indent(depth);
            printf("},\n");
            indent(depth);
            printf("field: %s,\n", expr->field_expr.name);
            depth--;
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_PATH:
            printf("path {\n");
            depth++;
            indent(depth);
            printf("stem: %s,\n", expr->path_expr.stem);
            indent(depth);
            printf("leaf {\n");
            print_expr(expr->path_expr.expr, depth + 1);
            indent(depth);
            printf("},\n");
            depth--;
            indent(depth);
            printf("},\n");
            break;

        case EXPR_KIND_CALL:
            vec_len = expr->call_expr.arg_vec.size;
            printf("call {\n");
            depth++;
            indent(depth);
            printf("object {\n");
            print_expr(expr->call_expr.object, depth + 1);
            indent(depth);
            printf("},\n");
            indent(depth);
            printf("args {\n");
            depth++;
            for (usize i = 0; i < vec_len; i++) {
                indent(depth);
                printf("arg_%zu {\n", i);
                expr_t arg_expr = *(expr_t*)vector_get(&expr->call_expr.arg_vec, i);
                print_expr(&arg_expr, depth + 1);
                indent(depth);
                printf("},\n");
            }
            depth--;
            indent(depth);
            printf("},\n");
            depth--;
            indent(depth);
            printf("},\n");
            break;

        default:
            printf("<UNSUPPORTED: %i>\n", expr->expr_kind);
            break;
    }
    // indent(depth);
}

u64 count_csv(const char* data, char sep) {
    // count the comma separated values before finding the right-hand version of the separator
    // todo: better string handling to allow regex
    assert(data != NULL);

    u64 count = 0;
    u64 arr[256] = {
        ['{'] = 0,
        ['('] = 0,
        ['['] = 0,
        ['"'] = 0,
    };
    u64 translate[256] = {
        ['}'] = '{',
        [')'] = '(',
        [']'] = '[',
        ['"'] = '"',
    };

    arr[(u8)sep]++;
    usize data_len = strlen(data);
    for (usize i = 0; i < data_len; i++) {
        if (data[i] == '"') {
            for (usize j = i + 1; j < data_len; j++) {
                if (!data[j]) {
                    panic("unexpected EOF during expression parse\n");
                }

                if (data[j] == '"') {
                    i = j;
                    break;
                }
            }
        }
        if (strchr("{([", data[i])) {
            arr[(u8)data[i]]++;
        }

        if (strchr("})]", data[i])) {
            arr[(u8)data[translate[i]]]--;
        }

        u64 arr_sum = 0;
        for (size_t j = 0; j < 256; j++) {
            arr_sum += arr[j];
        }

        if ((arr_sum == 1) && (data[i] == ',')) {
            count++;
        }

        if (!arr_sum) {
            break;
        }
    }

    return count;
}

expr_t* parse_expr_prec(arena_t* arena, parser_t* self, token_kind_t stop_token, expr_precedence_t min_prec) {
    // printf("\n");
    // printf("stop token: %s\n", token_kind_str(stop_token));
    arena_t local_arena = {};
    arena_init(&local_arena);

    token_t left_tok = lexer_peek(arena, &self->lexer);
    expr_t* lhs = arena_alloc(arena, sizeof(expr_t));
    expr_t* prev_lhs;

    if (left_tok.kind == stop_token) {
        // empty expression
        // log("lhs end of expression\n");
        lhs->expr_kind = EXPR_KIND_EMPTY;
        return lhs;
    }

    left_tok = lexer_next(arena, &self->lexer);

    if (left_tok.kind == TOK_IDENT) {
        // identifier
        // log("identifier expression\n");
        lhs->expr_kind = EXPR_KIND_IDENT;
        lhs->ident_expr = strdup2(arena, left_tok.str.data);

        // todo: missing byte and char literal support in AST
    } else if (left_tok.kind == TOK_FLOAT) {
        // float literal
        // log("float expression\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_FLOAT;
        lhs->literal_expr.float_ = strdup2(arena, left_tok.str.data);

    } else if (left_tok.kind == TOK_INTEGER) {
        // integer literal
        // log("integer expression\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_INTEGER;
        lhs->literal_expr.integer = strdup2(arena, left_tok.str.data);

    } else if (left_tok.kind == TOK_STRING) {
        // string literal
        // log("string expression\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_STRING;
        lhs->literal_expr.string = strdup2(arena, left_tok.str.data);

    } else if ((left_tok.kind == TOK_KW_FALSE) || (left_tok.kind == TOK_KW_TRUE)) {
        // log("bool expression\n");
        lhs->expr_kind = EXPR_KIND_LITERAL;
        lhs->literal_expr.kind = LITERAL_KIND_BOOL;
        lhs->literal_expr.bool_ = strdup2(arena, left_tok.str.data);

    } else if (left_tok.kind == TOK_LBRACK) {
        // array implicit literal, ex: [u32; 10]
        u64 comma_count = count_csv(self->lexer.data + self->lexer.meta.loc, '(');
        if (!comma_count) {
            // log("array implicit expression\n");
            lhs->expr_kind = EXPR_KIND_ARRAY_IMPLICIT;

            expr_t* element_type = parse_expr_prec(arena, self, TOK_SEMICOLON, 0);
            parser_expect(TOK_SEMICOLON, lexer_next(&local_arena, &self->lexer));
            expr_t* element_count = parse_expr_prec(arena, self, TOK_RBRACK, 0);
            parser_expect(TOK_RBRACK, lexer_next(&local_arena, &self->lexer));

            lhs->array_implicit_expr.kind = element_type;
            lhs->array_implicit_expr.size = element_count;

        } else {
            // array explicit literal, ex: [1, 2, 3]
            // log("array explicit expression\n");
            lhs->expr_kind = EXPR_KIND_ARRAY_EXPLICIT;
            for (u64 i = 0; i < comma_count; i++) {
                expr_t* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                vector_push(arena, &lhs->array_explicit_expr.arg_vec, arg_expr);
            }

            if (lexer_peek(arena, &self->lexer).kind != TOK_RBRACK) {
                expr_t* arg_expr = parse_expr_prec(arena, self, TOK_RBRACK, 0);
                parser_expect(TOK_RBRACK, lexer_next(&local_arena, &self->lexer));
                vector_push(arena, &lhs->array_explicit_expr.arg_vec, arg_expr);
            }
        }

    } else if (left_tok.kind == TOK_LPAREN) {
        // group expression
        // todo: support tuples, ex: (1, 2, 3)
        // log("group expression\n");
        lhs = parse_expr_prec(arena, self, TOK_RPAREN, 0);
        parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));

    } else if (is_valid_op(left_tok.kind)) {
        // unary expression
        // log("unary expression\n");
        lhs->expr_kind = EXPR_KIND_UNARY;
        lhs->unary_expr.op = *token_copy(arena, &left_tok);
        lhs->unary_expr.inner = parse_expr_prec(arena, self, stop_token, EXPR_PREC_PREFIX - 1);
        // parser_expect(stop_token, lexer_next(&local_arena, &self->lexer));
    }

    while (true) {
        // log("checking rhs\n");
        token_t operator_tok = lexer_peek(arena, &self->lexer);
        if (operator_tok.kind == stop_token) {
            // log("rhs end of expression\n");
            // end of expression
            break;
        }

        if (!is_valid_op(operator_tok.kind)) {
            // invalid operator
            panic("invalid token encountered during expression parse: '%s'", token_string(arena, &operator_tok));
        }

        expr_precedence_t postfix_prec = postfix_precedence(operator_tok.kind);
        if (postfix_prec) {
            if (postfix_prec < min_prec) {
                // log("postfix break\n");
                break;
            }

            operator_tok = lexer_next(arena, &self->lexer);

            if (operator_tok.kind == TOK_LBRACE) {
                // struct init expression
                prev_lhs = lhs;

                lhs = arena_alloc(arena, sizeof(expr_t));
                lhs->expr_kind = EXPR_KIND_STRUCT;

                lhs->struct_init_expr.object = prev_lhs;
                vector_init(arena, &lhs->struct_init_expr.arg_vec, sizeof(expr_t), 1);

                u64 comma_count = count_csv(self->lexer.data + self->lexer.meta.loc, '(');
                for (u64 i = 0; i < comma_count; i++) {
                    expr_t* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                    parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                    vector_push(arena, &lhs->struct_init_expr.arg_vec, arg_expr);
                }

                if (lexer_peek(arena, &self->lexer).kind != TOK_RBRACE) {
                    expr_t* arg_expr = parse_expr_prec(arena, self, TOK_RBRACE, 0);
                    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));
                    vector_push(arena, &lhs->struct_init_expr.arg_vec, arg_expr);
                }
                continue;

            } else if (operator_tok.kind == TOK_LPAREN) {
                // call or group expression
                if (lhs->expr_kind == EXPR_KIND_BINARY) {
                    // group expression
                    panic("unhandled binary expression preceding group - edge case\n");
                } else {
                    // call expression
                    prev_lhs = lhs;

                    lhs = arena_alloc(arena, sizeof(expr_t));
                    lhs->expr_kind = EXPR_KIND_CALL;
                    lhs->call_expr.object = prev_lhs;
                    vector_init(arena, &lhs->call_expr.arg_vec, sizeof(expr_t), 1);

                    u64 comma_count = count_csv(self->lexer.data + self->lexer.meta.loc, '(');
                    for (u64 i = 0; i < comma_count; i++) {
                        expr_t* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                        parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->call_expr.arg_vec, arg_expr);
                    }

                    if (lexer_peek(arena, &self->lexer).kind != TOK_RPAREN) {
                        expr_t* arg_expr = parse_expr_prec(arena, self, TOK_RPAREN, 0);
                        parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->call_expr.arg_vec, arg_expr);
                    }
                    continue;
                }

            } else if (operator_tok.kind == TOK_DECIMAL) {
                // field-type expression
                operator_tok = lexer_next(arena, &self->lexer); // field
                if (TOK_IDENT != operator_tok.kind) {
                    panic("expected field identifier after token '.' but found '%s'", operator_tok.str.data);
                }

                token_t lparen = lexer_peek(arena, &self->lexer); // possible method call
                if (lparen.kind == TOK_LPAREN) {
                    lparen = lexer_next(arena, &self->lexer); // possible method call
                    // method call expression
                    prev_lhs = lhs;

                    lhs = arena_alloc(arena, sizeof(expr_t));
                    vector_init(arena, &lhs->method_call_expr.arg_vec, sizeof(expr_t), 1);

                    lhs->expr_kind = EXPR_KIND_METHOD_CALL;
                    lhs->method_call_expr.object = prev_lhs;
                    lhs->method_call_expr.method = strdup2(arena, operator_tok.str.data);

                    u64 comma_count = count_csv(self->lexer.data + self->lexer.meta.loc, '(');
                    for (u64 i = 0; i < comma_count; i++) {
                        expr_t* arg_expr = parse_expr_prec(arena, self, TOK_COMMA, 0);
                        parser_expect(TOK_COMMA, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->method_call_expr.arg_vec, arg_expr);
                    }

                    if (lexer_peek(arena, &self->lexer).kind != TOK_RPAREN) {
                        expr_t* arg_expr = parse_expr_prec(arena, self, TOK_RPAREN, 0);
                        parser_expect(TOK_RPAREN, lexer_next(&local_arena, &self->lexer));
                        vector_push(arena, &lhs->method_call_expr.arg_vec, arg_expr);
                    }

                    continue;
                }

                prev_lhs = lhs;

                lhs = arena_alloc(arena, sizeof(expr_t));
                lhs->expr_kind = EXPR_KIND_FIELD;
                lhs->field_expr.object = prev_lhs;
                lhs->field_expr.name = strdup2(arena, operator_tok.str.data);
                continue;

            } else if (operator_tok.kind == TOK_LBRACK) {
                // array index expression
                prev_lhs = lhs;
                lhs = arena_alloc(arena, sizeof(expr_t));
                lhs->expr_kind = EXPR_KIND_ARRAY_INDEX;
                lhs->array_index_expr.object = prev_lhs;
                lhs->array_index_expr.index = parse_expr_prec(arena, self, TOK_RBRACK, EXPR_PREC_POSTFIX - 1);
                parser_expect(TOK_RBRACK, lexer_next(&local_arena, &self->lexer));
                continue;
            }
        }

        expr_precedence_t infix_prec = infix_precedence(operator_tok.kind);
        if (infix_prec) {
            if (infix_prec < min_prec) {
                break;
            }

            operator_tok = lexer_next(arena, &self->lexer);

            if (operator_tok.kind == TOK_PATH) {
                prev_lhs = lhs;
                lhs = arena_alloc(arena, sizeof(expr_t));
                lhs->expr_kind = EXPR_KIND_PATH;
                assert(prev_lhs->expr_kind == EXPR_KIND_IDENT);
                lhs->path_expr.stem = strdup2(arena, prev_lhs->ident_expr);
                lhs->path_expr.expr = parse_expr_prec(arena, self, stop_token, infix_prec - 1);
                continue;

            } else {
                prev_lhs = lhs;
                lhs = arena_alloc(arena, sizeof(expr_t));
                lhs->expr_kind = EXPR_KIND_BINARY;
                lhs->binary_expr.lhs = prev_lhs;
                lhs->binary_expr.op = *token_copy(arena, &operator_tok);
                lhs->binary_expr.rhs = parse_expr_prec(arena, self, stop_token, infix_prec - 1);
                continue;
            }
        }

        break;
    }

    arena_deinit(&local_arena);
    return lhs;
}

expr_t* parse_expr(arena_t* arena, parser_t* self, token_kind_t stop_token) {
    // log("entering parse expr\n");
    expr_t* expr = parse_expr_prec(arena, self, stop_token, 0);
    // log("leaving parse expr\n");
    return expr;
}

expr_t* visit_expr(arena_t* arena, parser_t* self, token_kind_t stop_token) {
    /*
     * Consume tokens until we reach the stop token, creating an expression tree
     */
    return parse_expr(arena, self, stop_token);
}

stmt_t visit_expr_stmt(arena_t* arena, parser_t* self) {
    stmt_t stmt = {};

    expr_t* expr = parse_expr(arena, self, TOK_SEMICOLON);
    parser_expect(TOK_SEMICOLON, lexer_next(arena, &self->lexer));

    stmt.kind = STMT_KIND_EXPR;
    stmt.expr_stmt.expr = expr;
    return stmt;
}

stmt_t visit_for_stmt(arena_t* arena, parser_t* self) {
    arena_t local_arena = {};
    stmt_t stmt = {};
    token_t feed = {};

    arena_init(&local_arena);

    parser_expect(TOK_KW_FOR, lexer_next(&local_arena, &self->lexer));

    feed = parser_expect(TOK_IDENT, lexer_next(&local_arena, &self->lexer));
    stmt.kind = STMT_KIND_FOR;

    stmt.for_stmt.iterator = strdup2(arena, feed.str.data);
    parser_expect(TOK_KW_IN, lexer_next(&local_arena, &self->lexer));
    stmt.for_stmt.iterable = parse_expr(arena, self, TOK_LBRACE);
    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
    stmt.for_stmt.body = visit_body(arena, self);
    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    arena_deinit(&local_arena);
    return stmt;
}

stmt_t visit_if_stmt(arena_t* arena, parser_t* self) {
    arena_t local_arena = {};
    stmt_t stmt = {};
    token_t feed = {};

    arena_init(&local_arena);

    // 'if' token
    feed = parser_expect(TOK_KW_IF, lexer_next(&local_arena, &self->lexer));

    stmt.kind = STMT_KIND_IF;
    stmt.if_stmt.condition = parse_expr(arena, self, TOK_LBRACE);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    stmt.if_stmt.body = visit_body(arena, self);
    vector_init(arena, &stmt.if_stmt.elif_clause_vec, sizeof(elif_clause_t), 1);

    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    while (true) {
        feed = lexer_peek(&local_arena, &self->lexer);
        if (feed.kind != TOK_KW_ELSE) {
            break;
        }

        parser_expect(TOK_KW_ELSE, lexer_next(&local_arena, &self->lexer));

        feed = lexer_peek(&local_arena, &self->lexer); // maybe has 'if'
        if (feed.kind == TOK_KW_IF) {
            elif_clause_t elif_clause = {};
            parser_expect(TOK_KW_IF, lexer_next(&local_arena, &self->lexer));
            elif_clause.condition = parse_expr(arena, self, TOK_LBRACE);
            parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
            elif_clause.body = visit_body(arena, self);
            parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));
            vector_push(arena, &stmt.if_stmt.elif_clause_vec, &elif_clause);

        } else {
            else_clause_t else_clause = {};
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

stmt_t visit_while_stmt(arena_t* arena, parser_t* self) {
    arena_t local_arena = {};
    stmt_t stmt = {};

    arena_init(&local_arena);

    parser_expect(TOK_KW_WHILE, lexer_next(&local_arena, &self->lexer));
    stmt.kind = STMT_KIND_WHILE;
    stmt.while_stmt.condition = parse_expr(arena, self, TOK_LBRACE);
    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));
    stmt.while_stmt.body = visit_body(arena, self);
    parser_expect(TOK_RBRACE, lexer_next(&local_arena, &self->lexer));

    arena_deinit(&local_arena);
    return stmt;
}

stmt_t visit_return_stmt(arena_t* arena, parser_t* self) {
    // log("entering return stmt\n");
    stmt_t stmt = {};

    parser_expect(TOK_KW_RETURN, lexer_next(arena, &self->lexer));

    stmt.kind = STMT_KIND_RETURN;
    stmt.return_stmt.expr = visit_expr(arena, self, TOK_SEMICOLON);

    parser_expect(TOK_SEMICOLON, lexer_next(arena, &self->lexer));

    // log("exiting return stmt\n");
    return stmt;
}

stmt_t visit_continue_stmt(arena_t* arena, parser_t* self) { todo(); }

stmt_t visit_assign_stmt(arena_t* arena, parser_t* self) {
    arena_t local_arena = {};
    arena_init(&local_arena);
    stmt_t stmt = {};

    stmt.kind = STMT_KIND_ASSIGN;

    bool mut = false;
    token_t feed = lexer_next(arena, &self->lexer);
    if (feed.kind == TOK_KW_VAR) {
        mut = true;
    } else {
        parser_expect(TOK_KW_LET, feed);
    }
    stmt.assign_stmt.mut = mut;

    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    stmt.assign_stmt.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_EQ, lexer_next(arena, &self->lexer));
    stmt.assign_stmt.expr = parse_expr(arena, self, TOK_SEMICOLON);
    parser_expect(TOK_SEMICOLON, lexer_next(&local_arena, &self->lexer));

    arena_deinit(&local_arena);
    return stmt;
}

body_t visit_body(arena_t* arena, parser_t* self) {
    // log("entering visit_body\n");
    body_t body = {};
    vector_init(arena, &body.stmts, sizeof(stmt_t), 4);

    stmt_t stmt = {};

    arena_t local_arena = {};
    arena_init(&local_arena);

    char* tok_str = NULL;

    while (true) {
        token_t feed = lexer_peek(&local_arena, &self->lexer);
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

vector_t visit_module_inner(arena_t* arena, parser_t* self, u8 is_source) {
    // log("entering visit_module_inner\n");

    token_t token = {};
    char* tok_str = NULL;
    arena_t local_arena = {};
    symbol_t symbol = {};
    vector_t symbol_vec = {};

    arena_init(&local_arena);
    vector_init(arena, &symbol_vec, sizeof(symbol_t), 8);

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

ast_t parser_parse(arena_t* arena, parser_t* self) {
    // log("entering parser_parse\n");
    ast_t ast = {};
    module_t module = {.name = "ANONYMOUS", visit_module_inner(arena, self, true)};
    vector_init(arena, &ast.module_vec, sizeof(module_t), 1);
    vector_push(arena, &ast.module_vec, &module);

    return ast;
}

void print_struct(struct_t* struct_, u64 depth) {
    indent(depth);
    printf("struct {\n");
    depth++;
    indent(depth);
    printf("ident: %s,\n", struct_->name);
    for (u32 i = 0; i < struct_->field_vec.size; i++) {
        indent(depth);
        struct_field_t* field = vector_get(&struct_->field_vec, i);
        xnotnull(field);
        printf("field { ident: %s, type: %s },\n", field->name, field->type.name);
    }
    for (usize j = 0; j < struct_->method_vec.size; j++) {
        symbol_t* symbol = vector_get(&struct_->method_vec, j);
        print_func(&symbol->func_case, depth);
    }
    depth--;
    indent(depth);
    printf("},\n");
}

symbol_t visit_struct(arena_t* arena, parser_t* self) {
    // log("visiting struct\n");

    arena_t local_arena = {};
    symbol_t symbol = {};
    struct_t struct_ = {};

    arena_init(&local_arena);
    vector_init(arena, &struct_.field_vec, sizeof(struct_field_t), 1);
    vector_init(arena, &struct_.method_vec, sizeof(symbol_t), 1);

    parser_expect(TOK_KW_STRUCT, lexer_next(&local_arena, &self->lexer));

    // struct name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    struct_.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_peek(arena, &self->lexer);

    // struct contents
    while (feed.kind != TOK_RBRACE) {
        if (feed.kind == TOK_IDENT) { // struct field
            feed = lexer_next(&local_arena, &self->lexer);
            struct_field_t field = {};

            field.name = strdup2(arena, feed.str.data);

            parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));

            feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
            field.type.name = strdup2(arena, feed.str.data);
            vector_push(arena, &struct_.field_vec, &field);

            parser_expect(TOK_SEMICOLON, lexer_next(&local_arena, &self->lexer));

        } else if (feed.kind == TOK_KW_FN) { // struct method
            symbol_t method = visit_func(arena, self);
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

void print_assign_stmt(assign_stmt_t* assign_stmt, u64 depth) {
    indent(depth);
    printf("assign_stmt {\n");
    depth++;
    indent(depth);
    printf("ident: %s,\n", assign_stmt->name);
    indent(depth);
    printf("mut: %i\n", assign_stmt->mut);
    indent(depth);
    printf("expr {\n");
    print_expr(assign_stmt->expr, depth + 1);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("},\n");
}

void print_return_stmt(return_stmt_t* return_stmt, u64 depth) {
    indent(depth);
    printf("return_stmt {\n");
    depth++;
    indent(depth);
    printf("expr {\n");
    print_expr(return_stmt->expr, depth + 1);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("},\n");
}

void print_expr_stmt(expr_stmt_t* expr_stmt, u64 depth) {
    indent(depth);
    printf("expr_stmt {\n");
    depth++;
    indent(depth);
    printf("expr {\n");
    print_expr(expr_stmt->expr, depth + 1);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("},\n");
}

void print_for_stmt(for_stmt_t* for_stmt, u64 depth) {
    indent(depth);
    printf("for_stmt {\n");
    depth++;
    indent(depth);
    printf("iterator = %s,\n", for_stmt->iterator);
    indent(depth);
    printf("iterable {\n");
    print_expr(for_stmt->iterable, depth + 1);
    indent(depth);
    printf("},\n");
    indent(depth);
    printf("body {\n");
    print_body(&for_stmt->body, depth + 1);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("},\n");
}

void print_while_stmt(while_stmt_t* while_stmt, u64 depth) {
    indent(depth);
    printf("while_stmt {\n");
    depth++;
    indent(depth);
    printf("condition {\n");
    print_expr(while_stmt->condition, depth + 1);
    indent(depth);
    printf("},\n");
    indent(depth);
    printf("body {\n");
    print_body(&while_stmt->body, depth);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("},\n");
}

void print_elif_clause(elif_clause_t* elif_clause, u64 depth) {
    indent(depth);
    printf("elif_clause {\n");
    depth++;
    indent(depth);
    printf("condition {\n");
    print_expr(elif_clause->condition, depth);
    indent(depth);
    printf("},\n");
    indent(depth);
    printf("body {\n");
    print_body(&elif_clause->body, depth + 1);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("},\n");
}

void print_else_clause(else_clause_t* else_clause, u64 depth) {
    indent(depth);
    printf("else_clause {\n");
    printf("body {\n");
    print_body(&else_clause->body, depth + 1);
    printf("},\n");
    indent(depth);
    printf("},\n");
}

void print_if_stmt(if_stmt_t* if_stmt, u64 depth) {
    indent(depth);
    printf("if_stmt {\n");
    depth++;
    indent(depth);
    printf("condition {\n");
    print_expr(if_stmt->condition, depth);
    indent(depth);
    printf("},\n");
    indent(depth);
    printf("body {\n");
    print_body(&if_stmt->body, depth + 1);
    indent(depth);
    printf("},\n");
    for (usize i = 0; i < if_stmt->elif_clause_vec.size; i++) {
        elif_clause_t elif_clause = *(elif_clause_t*)vector_get(&if_stmt->elif_clause_vec, i);
        print_elif_clause(&elif_clause, depth);
    }
    depth--;
    indent(depth);
    printf("},\n");
}

void print_break_stmt(break_stmt_t* break_stmt, u64 depth) {
    indent(depth);
    printf("break,");
}

void print_continue_stmt(continue_stmt_t* continue_stmt, u64 depth) {
    indent(depth);
    printf("continue,");
}

void print_body(body_t* body, u64 depth) {
    for (usize i = 0; i < body->stmts.size; i++) {
        stmt_t* stmt = vector_get(&body->stmts, i);
        switch (stmt->kind) {
            case STMT_KIND_ASSIGN:
                print_assign_stmt(&stmt->assign_stmt, depth + 1);
                break;
            case STMT_KIND_RETURN:
                print_return_stmt(&stmt->return_stmt, depth + 1);
                break;
            case STMT_KIND_EXPR:
                print_expr_stmt(&stmt->expr_stmt, depth + 1);
                break;
            case STMT_KIND_FOR:
                print_for_stmt(&stmt->for_stmt, depth + 1);
                break;
            case STMT_KIND_WHILE:
                print_while_stmt(&stmt->while_stmt, depth + 1);
                break;
            case STMT_KIND_IF:
                print_if_stmt(&stmt->if_stmt, depth + 1);
                break;
            case STMT_KIND_BREAK:
                print_break_stmt(&stmt->break_stmt, depth + 1);
                break;
            case STMT_KIND_CONTINUE:
                print_continue_stmt(&stmt->cont_stmt, depth + 1);
                break;
            case STMT_KIND_UNDEFINED:
                panic("undefined statement kind\n");
        }
    }
}

void print_module(module_t* module, u64 depth) {
    indent(depth);
    printf("module {\n");
    // depth++;
    for (usize i = 0; i < module->symbol_vec.size; i++) {
        symbol_t* symbol = vector_get(&module->symbol_vec, i);
        switch (symbol->kind) {
            case SYMBOL_KIND_MODULE:
                print_module(&symbol->module_case, depth + 1);
                continue;
            case SYMBOL_KIND_ENUM:
                print_enum(&symbol->enum_case, depth + 1);
                continue;
            case SYMBOL_KIND_FUNC:
                print_func(&symbol->func_case, depth + 1);
                continue;
            case SYMBOL_KIND_GLOBAL:
                print_global(&symbol->global_case, depth + 1);
                continue;
            case SYMBOL_KIND_STRUCT:
                print_struct(&symbol->struct_case, depth + 1);
                continue;
            case SYMBOL_KIND_IMPORT:
                print_import(&symbol->import_case, depth + 1);
                continue;
            case SYMBOL_KIND_UNDEFINED:
                break;
        }
    }
    // depth--;
    printf("}\n");
}

symbol_t visit_module(arena_t* arena, parser_t* self) {
    // log("visiting module\n");

    arena_t local_arena = {};
    symbol_t symbol = {};
    module_t module = {};

    vector_init(arena, &module.symbol_vec, sizeof(symbol_t), 8);

    parser_expect(TOK_KW_MODULE, lexer_next(arena, &self->lexer));

    // module name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
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

void print_func(func_t* func, u64 depth) {
    indent(depth);
    printf("func {\n");
    depth++;
    indent(depth);
    printf("ident: %s,\n", func->name);
    indent(depth);
    printf("params {\n");
    depth++;
    if (func->arg_vec.size) {
        for (usize idx = 0; idx < func->arg_vec.size; idx++) {
            func_arg_t* func_arg = vector_get(&func->arg_vec, idx);
            indent(depth);
            printf("param_%zu {%s: %s},\n", idx, func_arg->name, func_arg->type.name);
        }
    }
    depth--;
    indent(depth);
    printf("},\n");
    indent(depth);
    printf("return_type: %s,\n", func->ret_type.name);
    indent(depth);
    printf("body {\n");
    print_body(&func->body, depth + 1);
    indent(depth);
    printf("},\n");
    depth--;
    indent(depth);
    printf("}\n");
}

symbol_t visit_func(arena_t* arena, parser_t* self) {
    // log("visiting func\n");
    xnotnull(arena);
    xnotnull(self);

    arena_t local_arena = {};
    arena_init(&local_arena);

    symbol_t symbol = {};
    func_t func = {};

    parser_expect(TOK_KW_FN, lexer_next(&local_arena, &self->lexer));
    vector_init(arena, &func.arg_vec, sizeof(func_arg_t), 1);

    // function name
    token_t feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    func.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LPAREN, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(arena, &self->lexer);

    // function arguments
    while (feed.kind != TOK_RPAREN) {
        func_arg_t arg = {};
        arg.name = strdup2(arena, feed.str.data);
        parser_expect(TOK_COLON, lexer_next(&local_arena, &self->lexer));
        feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
        arg.type.name = strdup2(arena, feed.str.data);
        vector_push(arena, &func.arg_vec, &arg);

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

void print_global(global_t* global, u64 depth) {
    indent(depth);
    printf("global {\n");
    depth++;
    printf("ident: %s,\n", global->name);
    printf("mut: %i,\n", global->mut);
    print_expr(global->expr, depth + 1);
    depth--;
    printf("},\n");
}

symbol_t visit_global(arena_t* arena, parser_t* self, bool is_var) {
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

    token_t feed = {};
    arena_t local_arena = {};
    symbol_t symbol = {};
    global_t global = {};

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

symbol_t visit_enum(arena_t* arena, parser_t* self) {
    // log("visiting enum\n");
    xnotnull(arena);
    xnotnull(self);

    arena_t local_arena = {};
    symbol_t symbol = {};
    enum_t enum_ = {};
    token_t feed = {};

    arena_init(&local_arena);

    feed = parser_expect(TOK_IDENT, lexer_next(arena, &self->lexer));
    enum_.name = strdup2(arena, feed.str.data);

    parser_expect(TOK_LBRACE, lexer_next(&local_arena, &self->lexer));

    feed = lexer_next(&local_arena, &self->lexer);
    while (feed.kind != TOK_RBRACE) {
        if (feed.kind == TOK_EOF) {
            panic("invalid syntax\n");
        }

        enum_kind_t* enum_kind = arena_alloc(arena, sizeof(enum_kind_t));

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

symbol_t visit_import(arena_t* arena, parser_t* self) {
    // log("visiting import\n");
    todo();
    xnotnull(arena);
    xnotnull(self);
    symbol_t symbol = {};

    // log("leaving import\n");
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

// expr_precedence_t get_precedence(const expr_kind_t expr_kind, const char* operator) {
//     switch (expr_kind) {
//     case EXPR_KIND_BINARY:
//         if (streq("||", operator)) {
//             return EXPR_PREC_LOR;
//         } else if (streq("&&", operator)) {
//             return EXPR_PREC_LAND;
//         } else if (streq("==", operator) || streq("!=", operator) || streq("<", operator) || streq(">", operator) ||
//                    streq("<=", operator) || streq(">=", operator)) {
//             return EXPR_PREC_COMP;
//         } else if (streq("|", operator)) {
//             return EXPR_PREC_BOR;
//         } else if (streq("^", operator)) {
//             return EXPR_PREC_BXOR;
//         } else if (streq("&", operator)) {
//             return EXPR_PREC_BAND;
//         } else if (streq(">>", operator) || streq(">>", operator)) {
//             return EXPR_PREC_SHIFT;
//         } else if (streq("+", operator) || streq("-", operator)) {
//             return EXPR_PREC_ADDSUB;
//         } else if (streq("*", operator) || streq("/", operator)) {
//             return EXPR_PREC_MULDIV;
//         }
//
//     case EXPR_KIND_ASSIGN:
//         return EXPR_PREC_ASSIGN;
//
//     case EXPR_KIND_RANGE:
//         return EXPR_PREC_RANGE;
//
//     case EXPR_KIND_UNARY:
//         return EXPR_PREC_PREFIX;
//
//     case EXPR_KIND_STRUCT:
//     case EXPR_KIND_ARRAY_INDEX:
//     case EXPR_KIND_CALL:
//     case EXPR_KIND_FIELD:
//     case EXPR_KIND_METHOD_CALL:
//         return EXPR_PREC_POSTFIX;
//
//     case EXPR_KIND_CLOSURE:
//     case EXPR_KIND_TUPLE:
//     case EXPR_KIND_GROUP:
//     case EXPR_KIND_ARRAY:
//     case EXPR_KIND_PATH:
//     case EXPR_KIND_LITERAL:
//     case EXPR_KIND_IDENT:
//         return EXPR_PREC_UNAMBIGUOUS;
//
//     default:
//         return EXPR_PREC_UNAMBIGUOUS;
//
//         // case EXPR_KIND_BREAK:
//         // case EXPR_KIND_CONTINUE:
//         // case EXPR_KIND_RETURN:
//         //     return EXPR_PREC_JUMP;
//     }
//
