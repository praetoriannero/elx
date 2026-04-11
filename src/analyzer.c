#include <glib.h>
#include <stdio.h>

#include "analyzer.h"
#include "parser.h"

// idea: table of trait impls for built-ins just like we did with the operator
// lookup tables
//
// check if type exists in the built-in table, then do a look-up to see if the
// operation is supported

void check_assign_stmt(AssignStmt* node, AstContext* ctx) {}

void check_continue_stmt(ContinueStmt* node, AstContext* ctx) {}

void check_expr_stmt(ExprStmt* node, AstContext* ctx) {}

void check_for_stmt(ForStmt* node, AstContext* ctx) {}

void check_if_stmt(IfStmt* node, AstContext* ctx) {}

void check_return_stmt(ReturnStmt* node, AstContext* ctx) {}

void check_while_stmt(WhileStmt* node, AstContext* ctx) {}

void check_body(Body* body, AstContext* ctx) {}

void check_main(Func* main_func) {}

void check_global(Global* global, AstContext* ctx) {}

void check_struct(Struct* struct_, AstContext* ctx) {}

void check_enum(Enum* enum_, AstContext* ctx) {}

void check_func(Func* func, AstContext* ctx) {}

void check_module(Module* module, AstContext* ctx) {
  /*
   * Check for redefinitions of symbols
   */
  GHashTable* symbol_set = g_hash_table_new(g_str_hash, g_str_equal);

  for (usize idx = 0; idx < module->symbol_vec.size; idx++) {
    Symbol* symbol = vector_get(&module->symbol_vec, idx);
    printf("validating symbol '%s'\n", symbol->name);
    if (g_hash_table_contains(symbol_set, symbol->name)) {
      panic("redefinition of symbol '%s'\n", symbol->name);
    }

    g_hash_table_add(symbol_set, symbol->name);

    switch (symbol->kind) {
      case SYMBOL_KIND_GLOBAL:
        check_global(&symbol->global_case, ctx);
        break;
      case SYMBOL_KIND_STRUCT:
        check_struct(&symbol->struct_case, ctx);
        break;
      case SYMBOL_KIND_ENUM:
        check_enum(&symbol->enum_case, ctx);
        break;
      case SYMBOL_KIND_MODULE:
        check_module(&symbol->module_case, ctx);
        break;
      case SYMBOL_KIND_FUNC:
        check_func(&symbol->func_case, ctx);
        break;
      case SYMBOL_KIND_UNDEFINED:
      default:
        panic("invalid symbol: '%s'\n");
    }
  }
}

void check_ast(Ast* ast, AstContext* ctx) {
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
  bool found_main = false;

  for (usize idx = 0; idx < ast->module_vec.size; idx++) {
    Module* module = vector_get(&ast->module_vec, idx);
    check_module(module, ctx);
  }

  if (ctx->is_lib && found_main) {
    panic("Missing program entry point 'main'\n");
  }
}

Type deduce_type(Expr* expr);
