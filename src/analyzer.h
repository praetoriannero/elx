#pragma once

#include "parser.h"

typedef struct {
    bool is_lib;
    vector_t loaded_modules_vec;
    vector_t scope_vec;
} context_t;

void check_expr(expr_t* expr, context_t* context);

void check_assign_stmt(assign_stmt_t* node, context_t* context);
void check_continue_stmt(continue_stmt_t* node, context_t* context);
void check_expr_stmt(expr_stmt_t* node, context_t* context);
void check_for_stmt(for_stmt_t* node, context_t* context);
void check_if_stmt(if_stmt_t* node, context_t* context);
void check_return_stmt(return_stmt_t* node, context_t* context);
void check_while_stmt(while_stmt_t* node, context_t* context);

void check_body(body_t* node, context_t* context);
void check_enum(enum_t* node, context_t* context);
void check_expr(expr_t* node, context_t* context);
void check_func(func_t* node, context_t* context);
void check_global(global_t* node, context_t* context);
void check_import(import_t* node, context_t* context);
void check_module(module_t* node, context_t* context);
void check_struct(struct_t* node, context_t* context);
void check_ast(ast_t* node, context_t* context);
