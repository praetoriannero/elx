#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "token.h"

typedef struct elx_path {
    char** paths;
    uint32_t path_count;
} elx_path_t;

typedef struct elx_ident {
    char* name;
    elx_path_t path;
} elx_ident_t;

typedef struct elx_struct_field {
    char* name;
    elx_ident_t type;
} elx_struct_field_t;

typedef struct elx_struct {
    elx_ident_t ident;
    elx_struct_field_t* fields;
    uint32_t field_count;
} elx_struct_t;

typedef struct elx_func_arg {
    char* name;
    elx_ident_t type;
} elx_func_arg_t;

typedef struct elx_expr {
    token_t* tokens;
    uint32_t token_count;
} elx_expr_t;

typedef struct elx_expr_stmt {
    elx_expr_t expr;
} elx_expr_stmt_t;

typedef enum elx_assign_op_kind {
    ELX_PLUS_EQUAL,
    ELX_MINUS_EQUAL,
    ELX_TIMES_EQUAL,
    ELX_DIVIDE_EQUAL,
    ELX_MOD_EQUAL,
    ELX_SHL_EQUAL,
    ELX_SHR_EQUAL,
    ELX_AND_EQUAL,
    ELX_OR_EQUAL,
    ELX_XOR_EQUAL,
    ELX_EQUAL,
} elx_assign_op_kind_t;

typedef struct elx_assign_stmt {
    char* name;
    elx_assign_op_kind_t assign_op;
    elx_expr_t expr;
} elx_assign_stmt_t;

// typedef struct elx_variable {
//     elx_ident_t ident;
//     bool mut;
//     elx_ident_t type;
// } elx_variable_t;

struct elx_stmt;

typedef struct {
    struct elx_stmt* stmts;
    uint32_t stmt_count;
} elx_body_t;

typedef struct {
    char* name;
    elx_expr_t expr;
} elx_let_stmt_t;

typedef struct {
    char* name;
    elx_expr_t expr;
} elx_var_stmt_t;

typedef struct {
    elx_expr_t expr;
} elx_return_stmt_t;

typedef struct {
    char* iterator;
    elx_expr_t iterable;
    elx_body_t body;
} elx_for_stmt_t;

typedef struct {
    elx_expr_t condition;
    elx_body_t body;
} elx_while_stmt_t;

typedef struct {
    elx_body_t body;
} elx_else_clause_t;

typedef struct {
    elx_expr_t condition;
    elx_body_t body;
} elx_elif_clause_t;

typedef struct {
    elx_expr_t condition;
    elx_body_t body;
    elx_elif_clause_t* elif_clauses;
    uint32_t elif_clause_count;
    elx_else_clause_t else_clause;
} elx_if_stmt_t;

typedef struct {
    elx_ident_t* ident;
} elx_break_stmt_t;

typedef struct {
    elx_ident_t* ident;
} elx_continue_stmt_t;

struct elx_symbol;

typedef struct {
    elx_ident_t ident;
    struct elx_symbol* symbols;
    uint32_t symbol_count;
} elx_module_stmt;

typedef enum {
    ELX_ASSIGN_STMT,
    ELX_LET_STMT,
    ELX_VAR_STMT,
    ELX_RETURN_STMT,
    ELX_EXPR_STMT,
    ELX_FOR_STMT,
    ELX_WHILE_STMT,
    ELX_IF_STMT,
    ELX_BREAK_STMT,
    ELX_CONTINUE_STMT,
    ELX_MODULE_STMT,
} elx_stmt_kind_t;

typedef union {
    elx_expr_stmt_t expr_stmt;
    elx_assign_stmt_t assign_stmt;
} elx_stmt_variant_t;

typedef struct {
    elx_stmt_kind_t kind;
    elx_stmt_variant_t variant;
} elx_stmt_t;

typedef struct {
    elx_ident_t ident;
    elx_func_arg_t* args;
    uint32_t arg_count;
    elx_body_t body;
} elx_func_t;

typedef enum {
    ELX_STRUCT_KIND,
    ELX_GLOBAL_KIND,
    ELX_ENUM_KIND,
    ELX_MODULE_KIND,
    ELX_FUNC_KIND,
    ELX_IMPORT_KIND,
} elx_symbol_kind_t;

typedef union {
    elx_struct_t struct_kind;
} elx_symbol_variant_t;

typedef struct {
    elx_symbol_kind_t kind;
    elx_symbol_variant_t variant;
} elx_symbol_t;

typedef struct {
    elx_ident_t ident;
} elx_module_t;
