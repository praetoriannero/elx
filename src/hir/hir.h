/*  HIR performs the following:
 *  - macro expansion
 *  - desugaring
 *  - type inference
 *  - type checking
 *  
 */
#pragma once

#include "parser/parser.h"

typedef struct {
  Ast* ast;
} HirContext;

Ast* hir_lower_ast(HirContext* self, Ast* ast);

/*  @brief HIR visitor functions
 */

void hir_visit_expr(HirContext* self, Expr* expr);
void hir_visit_assign_stmt(HirContext* self, AssignStmt* node);
void hir_visit_continue_stmt(HirContext* self, ContinueStmt* node);
void hir_visit_expr_stmt(HirContext* self, ExprStmt* node);
void hir_visit_for_stmt(HirContext* self, ForStmt* node);
void hir_visit_if_stmt(HirContext* self, IfStmt* node);
void hir_visit_return_stmt(HirContext* self, ReturnStmt* node);
void hir_visit_while_stmt(HirContext* self, WhileStmt* node);
void hir_visit_body(HirContext* self, Body* node);
void hir_visit_enum(HirContext* self, Enum* node);
void hir_visit_expr(HirContext* self, Expr* node);
void hir_visit_func(HirContext* self, Func* node);
void hir_visit_global(HirContext* self, Global* node);
void hir_visit_import(HirContext* self, Import* node);
void hir_visit_module(HirContext* self, Module* node);
void hir_visit_struct(HirContext* self, Struct* node);
void hir_visit_ast(HirContext* self, Ast* node);
