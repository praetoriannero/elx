#include <glib.h>
#include <stdio.h>

#include "analyzer.h"
#include "allocator.h"
#include "parser.h"

// idea: table of trait impls for built-ins just like we did with the operator
// lookup tables
//
// check if type exists in the built-in table, then do a look-up to see if the
// operation is supported

void analyzer_visit_expr(Expr* expr, AnalyzerContext *ctx) {}

void analyzer_visit_assign_stmt(AssignStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_continue_stmt(ContinueStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_expr_stmt(ExprStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_for_stmt(ForStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_if_stmt(IfStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_return_stmt(ReturnStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_while_stmt(WhileStmt* node, AnalyzerContext* ctx) {}

void analyzer_visit_body(Body* node, AnalyzerContext* ctx) {}

void analyzer_visit_global(Global* global, AnalyzerContext* ctx) {}

void analyzer_visit_struct(Struct* struct_, AnalyzerContext* ctx) {}

void analyzer_visit_enum(Enum* enum_, AnalyzerContext* ctx) {}

void analyzer_visit_func(Func* func, AnalyzerContext* ctx) {}

void analyzer_visit_module(Module* module, AnalyzerContext* ctx) {
  GHashTable* ast_node_set = g_hash_table_new(g_str_hash, g_str_equal);

  VectorIter vec_iter = {};
  vector_iter_init(&vec_iter, &module->ast_node_vec);
  void* element = NULL;

  while (vector_iter_next(&vec_iter, &element)) {
    AstNode* ast_node = (AstNode*)element;

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
        analyzer_visit_module(&ast_node->module_case, ctx);
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
  bool found_main = false;
  Allocator alloc = {};
  allocator_init(&alloc);

  ctx->global_scope = allocator_alloc(&alloc, sizeof(Scope));
  ctx->global_scope->symbol_table = g_hash_table_new(g_str_hash, g_str_equal);

  for (usize idx = 0; idx < ast->module_vec.size; idx++) {
    Module* module = vector_get(&ast->module_vec, idx);
    analyzer_visit_module(module, ctx);
  }

  GHashTableIter hash_iter;
  gpointer key = NULL;
  gpointer value = NULL;
  g_hash_table_iter_init(&hash_iter, ctx->global_scope->symbol_table);
  while (g_hash_table_iter_next(&hash_iter, &key, &value)) {
    char* key_str = (char*)key;
    if (strcmp(key_str, "main")) {
      found_main = true;
    }
  }

  // if (!found_main) {
  //   panic("Missing program entry point 'main'\n");
  // }

  g_hash_table_destroy(ctx->global_scope->symbol_table);
  allocator_deinit(&alloc);
}

bool verify_type(Type* type_);

Type resolve_expr_type(Expr* expr);
