#pragma once

#include "parser.h"

typedef struct {
  bool is_lib;
  Vector global_scope_vec;
  Vector local_scope_vec;
} AnalyzerContext;

void analyzer_check_expr(Expr* expr, AnalyzerContext* ctx);

void analyzer_check_assign_stmt(AssignStmt* node, AnalyzerContext* ctx);
void analyzer_check_continue_stmt(ContinueStmt* node, AnalyzerContext* ctx);
void analyzer_check_expr_stmt(ExprStmt* node, AnalyzerContext* ctx);
void analyzer_check_for_stmt(ForStmt* node, AnalyzerContext* ctx);
void analyzer_check_if_stmt(IfStmt* node, AnalyzerContext* ctx);
void analyzer_check_return_stmt(ReturnStmt* node, AnalyzerContext* ctx);
void analyzer_check_while_stmt(WhileStmt* node, AnalyzerContext* ctx);

void analyzer_check_body(Body* node, AnalyzerContext* ctx);
void analyzer_check_enum(Enum* node, AnalyzerContext* ctx);
void analyzer_check_expr(Expr* node, AnalyzerContext* ctx);
void analyzer_check_func(Func* node, AnalyzerContext* ctx);
void analyzer_check_global(Global* node, AnalyzerContext* ctx);
void analyzer_check_import(Import* node, AnalyzerContext* ctx);
void analyzer_check_module(Module* node, AnalyzerContext* ctx);
void analyzer_check_struct(Struct* node, AnalyzerContext* ctx);
void analyzer_check_ast(Ast* node, AnalyzerContext* ctx);
