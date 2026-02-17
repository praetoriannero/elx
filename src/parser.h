#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lexer.h"
#include "vector.h"

typedef struct {
    char* name;
    vector_t path;
} ident_t;

typedef struct {
    char* name;
    ident_t type;
} struct_field_t;

typedef struct {
    ident_t ident;
    vector_t fields;
} struct_t;

typedef struct {
    char* name;
    ident_t type;
} func_arg_t;

typedef struct {
    vector_t tokens;
} expr_t;

typedef struct {
    expr_t expr;
} expr_stmt_t;

typedef enum {
    PLUS_EQUAL,
    MINUS_EQUAL,
    TIMES_EQUAL,
    DIVIDE_EQUAL,
    MOD_EQUAL,
    SHL_EQUAL,
    SHR_EQUAL,
    AND_EQUAL,
    OR_EQUAL,
    XOR_EQUAL,
    EQUAL,
} assign_op_kind_t;

typedef struct {
    char* name;
    assign_op_kind_t assign_op;
    expr_t expr;
} assign_stmt_t;

struct stmt;

typedef struct {
    vector_t stmts;
} body_t;

typedef struct {
    char* name;
    expr_t expr;
} let_stmt_t;

typedef struct {
    char* name;
    expr_t expr;
} var_stmt_t;

typedef struct {
    expr_t expr;
} return_stmt_t;

typedef struct {
    char* iterator;
    expr_t iterable;
    body_t body;
} for_stmt_t;

typedef struct {
    expr_t condition;
    body_t body;
} while_stmt_t;

typedef struct {
    body_t body;
} else_clause_t;

typedef struct {
    expr_t condition;
    body_t body;
} elif_clause_t;

typedef struct {
    expr_t condition;
    body_t body;
    elif_clause_t* elif_clauses;
    uint32_t elif_clause_count;
    else_clause_t else_clause;
} if_stmt_t;

typedef struct {
    ident_t* ident;
} break_stmt_t;

typedef struct {
    ident_t* ident;
} continue_stmt_t;

struct symbol;

typedef struct {
    ident_t ident;
    vector_t symbol_vec;
    // struct symbol* symbols;
    // uint32_t symbol_count;
} module_t;

typedef enum {
    ASSIGN_STMT,
    LET_STMT,
    VAR_STMT,
    RETURN_STMT,
    EXPR_STMT,
    FOR_STMT,
    WHILE_STMT,
    IF_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    MODULE_STMT,
} stmt_kind_t;

typedef union {
    expr_stmt_t expr_stmt;
    assign_stmt_t assign_stmt;
} stmt_variant_t;

typedef struct {
    stmt_kind_t kind;
    stmt_variant_t variant;
} stmt_t;

typedef struct {
    ident_t ident;
    func_arg_t* args;
    uint32_t arg_count;
    body_t body;
    ident_t ret_type;
} func_t;

typedef struct {
    char* name;
    bool mut;
    ident_t type;
    expr_t expr;
} global_t;

typedef struct {
    char* name;
    ident_t* types;
    uint32_t type_count;
} enum_t;

typedef struct {
    ident_t path;
} import_t;

typedef enum {
    SYMBOL_UNDEFINED,
    SYMBOL_STRUCT,
    SYMBOL_MODULE,
    SYMBOL_FUNC,
    SYMBOL_GLOBAL,
    SYMBOL_ENUM,
    SYMBOL_IMPORT,
} symbol_kind_t;

typedef union {
    struct_t struct_case;
    module_t module_case;
    func_t func_case;
    global_t global_case;
    enum_t enum_case;
    import_t import_case;
} symbol_union_t;

typedef struct {
    symbol_kind_t kind;
    symbol_union_t variant;
    vector_t path;
} symbol_t;

typedef struct {
    symbol_t* symbols;
    uint32_t symbol_count;
} ast_t;

typedef struct parser {
    lexer_t lexer;
    ast_t (*parse)(struct parser* self);
    symbol_t* (*visit_struct)(struct parser* self);
    symbol_t* (*visit_module)(struct parser* self);
    symbol_t* (*visit_func)(struct parser* self);
    symbol_t* (*visit_global)(struct parser* self, bool mut);
    symbol_t* (*visit_enum)(struct parser* self);
    symbol_t* (*visit_import)(struct parser* self);
} parser_t;

ast_t parser_parse(parser_t* self);

// symbol_t* parser_visit_struct(parser_t* self);
//
// symbol_t* parser_visit_module(parser_t* self);
//
// symbol_t* parser_visit_func(parser_t* self);
//
// symbol_t* parser_visit_global(parser_t* self);
//
// symbol_t* parser_visit_enum(parser_t* self);
//
// symbol_t* parser_visit_import(parser_t* self);
//
parser_t* parser_new(lexer_t lexer);
//
// token_t parser_expect(token_kind_t expected, token_t actual);

void parser_init(parser_t* self, lexer_t lexer);

void parser_deinit(parser_t* self, lexer_t lexer);

void parser_free(parser_t* self);
