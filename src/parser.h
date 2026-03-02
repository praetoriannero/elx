#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lexer.h"
#include "vector.h"

typedef enum {
    TYPE_KIND_RAW,
    TYPE_KIND_PTR,
    TYPE_KIND_REF,
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
    ASSIGN_OP_KIND_PLUS_EQUAL,
    ASSIGN_OP_KIND_MINUS_EQUAL,
    ASSIGN_OP_KIND_TIMES_EQUAL,
    ASSIGN_OP_KIND_DIVIDE_EQUAL,
    ASSIGN_OP_KIND_MOD_EQUAL,
    ASSIGN_OP_KIND_SHL_EQUAL,
    ASSIGN_OP_KIND_SHR_EQUAL,
    ASSIGN_OP_KIND_AND_EQUAL,
    ASSIGN_OP_KIND_OR_EQUAL,
    ASSIGN_OP_KIND_XOR_EQUAL,
    ASSIGN_OP_KIND_EQUAL,
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
    STMT_KIND_UNDEFINED,
    STMT_KIND_ASSIGN,
    STMT_KIND_LET,
    STMT_KIND_RETURN,
    STMT_KIND_EXPR,
    STMT_KIND_FOR,
    STMT_KIND_WHILE,
    STMT_KIND_IF,
    STMT_KIND_BREAK,
    STMT_KIND_CONTINUE,
} stmt_kind_t;

typedef union {
    assign_stmt_t assign_stmt;
    let_stmt_t let_stmt;
    return_stmt_t return_stmt;
    expr_stmt_t expr_stmt;
    for_stmt_t for_stmt;
    while_stmt_t while_stmt;
    if_stmt_t if_stmt;
    break_stmt_t break_stmt;
    continue_stmt_t cont_stmt;
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
    SYMBOL_KIND_UNDEFINED,
    SYMBOL_KIND_STRUCT,
    SYMBOL_KIND_MODULE,
    SYMBOL_KIND_FUNC,
    SYMBOL_KIND_GLOBAL,
    SYMBOL_KIND_ENUM,
    SYMBOL_KIND_IMPORT,
    SYMBOL_KIND_IMPL,
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
