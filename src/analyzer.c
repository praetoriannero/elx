#include <glib.h>
#include <stdio.h>

#include "analyzer.h"
#include "parser.h"

// idea: table of trait impls for built-ins just like we did with the operator
// lookup tables
//
// check if type exists in the built-in table, then do a look-up to see if the
// operation is supported

void analyzer_visit_assign_stmt(AssignStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_continue_stmt(ContinueStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_expr_stmt(ExprStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_for_stmt(ForStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_if_stmt(IfStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_return_stmt(ReturnStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_while_stmt(WhileStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_body(Body* node, AnalyzerContext* ctx, Scope* parent_scope) {}

void analyzer_visit_main(Func* main_func) {}

void analyzer_visit_global(Global* global, AnalyzerContext* ctx) {}

void analyzer_visit_struct(Struct* struct_, AnalyzerContext* ctx) {}

void analyzer_visit_enum(Enum* enum_, AnalyzerContext* ctx) {}

void analyzer_visit_func(Func* func, AnalyzerContext* ctx) {}

void analyzer_visit_module(Module* module, AnalyzerContext* ctx, Scope* parent_scope) {
  /*
   * Check for redefinitions of symbols
   */
  GHashTable* ast_node_set = g_hash_table_new(g_str_hash, g_str_equal);
  Scope module_scope = {};

  for (usize idx = 0; idx < module->ast_node_vec.size; idx++) {
    AstNode* ast_node = vector_get(&module->ast_node_vec, idx);
    printf("validating symbol '%s'\n", ast_node->name);
    if (g_hash_table_contains(ast_node_set, ast_node->name)) {
      panic("redefinition of symbol '%s'\n", ast_node->name);
    }

    g_hash_table_add(ast_node_set, ast_node->name);

    switch (ast_node->kind) {
      case AST_NODE_KIND_GLOBAL:
        analyzer_visit_global(&ast_node->global_case, ctx);
        break;
      case AST_NODE_KIND_STRUCT:
        analyzer_visit_struct(&ast_node->struct_case, ctx);
        break;
      case AST_NODE_KIND_ENUM:
        analyzer_visit_enum(&ast_node->enum_case, ctx);
        break;
      case AST_NODE_KIND_MODULE:
        analyzer_visit_module(&ast_node->module_case, ctx, &module_scope);
        break;
      case AST_NODE_KIND_FUNC:
        analyzer_visit_func(&ast_node->func_case, ctx);
        break;
      case AST_NODE_KIND_UNDEFINED:
      default:
        panic("invalid symbol: '%s'\n");
    }
  }
}

void analyzer_visit_ast(Ast* ast, AnalyzerContext* ctx) {
  /*
   * Check that main exists as a function
   *
   * Check that main is correct
   *      - proper arguments
   *      - proper return type
   *
   * Check symbols
   *
   */
  // bool found_main = false;

  Scope global_scope = {};
  ctx->global_scope = global_scope;

  for (usize idx = 0; idx < ast->module_vec.size; idx++) {
    Module* module = vector_get(&ast->module_vec, idx);
    analyzer_visit_module(module, ctx, &ctx->global_scope);
  }

  // if (!found_main) {
  //   panic("Missing program entry point 'main'\n");
  // }
}

bool verify_type(Type* type_);

Type resolve_expr_type(Expr* expr);
