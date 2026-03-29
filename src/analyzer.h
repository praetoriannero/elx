#pragma once

#include "parser.h"

typedef struct {
    bool is_lib;
    Vector loaded_modules_vec;
    Vector scope_vec;
} AstContext;

void check_expr(Expr* expr, AstContext* context);

void check_assign_stmt(AssignStmt* node, AstContext* context);
void check_continue_stmt(ContinueStmt* node, AstContext* context);
void check_expr_stmt(ExprStmt* node, AstContext* context);
void check_for_stmt(ForStmt* node, AstContext* context);
void check_if_stmt(IfStmt* node, AstContext* context);
void check_return_stmt(ReturnStmt* node, AstContext* context);
void check_while_stmt(WhileStmt* node, AstContext* context);

void check_body(Body* node, AstContext* context);
void check_enum(Enum* node, AstContext* context);
void check_expr(Expr* node, AstContext* context);
void check_func(Func* node, AstContext* context);
void check_global(Global* node, AstContext* context);
void check_import(Import* node, AstContext* context);
void check_module(Module* node, AstContext* context);
void check_struct(Struct* node, AstContext* context);
void check_ast(Ast* node, AstContext* context);
