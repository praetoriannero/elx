#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lexer.h"
#include "vector.h"

typedef enum {
    TYPE_RAW,
    TYPE_PTR,
    TYPE_REF,
} type_kind_t;

typedef struct {
    char* name;
    vector_t path;
} ident_t;

typedef struct type_ {
    type_kind_t kind;
    ident_t ident;
} type_t;

typedef struct {
    char* name;
    type_t type;
} struct_field_t;

typedef struct {
    ident_t ident;
    vector_t field_vec;
} struct_t;

typedef struct {
    char* name;
    type_t type;
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
    vector_t elif_clause_vec;
    else_clause_t else_clause;
} if_stmt_t;

typedef struct {
    ident_t ident;
} break_stmt_t;

typedef struct {
    ident_t ident;
} continue_stmt_t;

struct symbol;

typedef struct {
    ident_t ident;
    vector_t symbol_vec;
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
    vector_t arg_vec;  // vec<func_arg_t>
    body_t body;
    type_t ret_type;
} func_t;

typedef struct {
    char* name;
    bool mut;
    ident_t type;
    expr_t expr;
} global_t;

typedef enum {
    ENUM_VARIANT_NOM,
    ENUM_VARIANT_ALG,
} enum_variant_t;

typedef struct {
    char* name;
    type_t type;
    enum_variant_t variant;
} enum_kind_t;

typedef struct {
    ident_t ident;
    vector_t kind_vec;  // vec<enum_kind_t>
} enum_t;

typedef struct {
    ident_t ident;
    vector_t fn_vec;
} impl_t;

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
    SYMBOL_IMPL,
} symbol_kind_t;

typedef union {
    struct_t struct_case;
    module_t module_case;
    func_t func_case;
    global_t global_case;
    enum_t enum_case;
    import_t import_case;
    impl_t impl_case;
} symbol_union_t;

typedef struct {
    symbol_kind_t kind;
    symbol_union_t variant;
    vector_t path;
} symbol_t;

typedef struct {
    vector_t module_vec;
} ast_t;

typedef struct parser {
    lexer_t lexer;
} parser_t;

ast_t parser_parse(arena_t* arena, parser_t* self);

parser_t* parser_new(arena_t* arena, lexer_t lexer);

void parser_init(parser_t* self, lexer_t lexer);

// void parser_deinit(parser_t* self, lexer_t lexer);

// void parser_free(parser_t* self);
