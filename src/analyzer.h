#pragma once

#include <glib.h>

#include "parser.h"

typedef struct Scope {
  GHashTable* symbol_table;   // &GHashTable<char*, &Symbol>
  Vector child_scopes;        // Vector<Scope>
  struct Scope* parent_scope; // null if top scope
} Scope;

typedef struct {
  Scope global_scope;
  Scope local_scope;
} AnalyzerContext;

void analyzer_visit_expr(Expr* expr, AnalyzerContext* ctx);

void analyzer_visit_assign_stmt(AssignStmt* node, AnalyzerContext* ctx);
void analyzer_visit_continue_stmt(ContinueStmt* node, AnalyzerContext* ctx);
void analyzer_visit_expr_stmt(ExprStmt* node, AnalyzerContext* ctx);
void analyzer_visit_for_stmt(ForStmt* node, AnalyzerContext* ctx);
void analyzer_visit_if_stmt(IfStmt* node, AnalyzerContext* ctx);
void analyzer_visit_return_stmt(ReturnStmt* node, AnalyzerContext* ctx);
void analyzer_visit_while_stmt(WhileStmt* node, AnalyzerContext* ctx);

void analyzer_visit_body(Body* node, AnalyzerContext* ctx, Scope* parent_scope);
void analyzer_visit_enum(Enum* node, AnalyzerContext* ctx);
void analyzer_visit_expr(Expr* node, AnalyzerContext* ctx);
void analyzer_visit_func(Func* node, AnalyzerContext* ctx);
void analyzer_visit_global(Global* node, AnalyzerContext* ctx);
void analyzer_visit_import(Import* node, AnalyzerContext* ctx);
void analyzer_visit_module(Module* module, AnalyzerContext* ctx, Scope* parent_scope);
void analyzer_visit_struct(Struct* node, AnalyzerContext* ctx);
void analyzer_visit_ast(Ast* node, AnalyzerContext* ctx);
