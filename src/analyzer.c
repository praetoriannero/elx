#include <glib.h>

#include "analyzer.h"
#include "parser.h"

void check_main(Func* main_func) {}

// idea: table of trait impls for built-ins just like we did with the operator lookup tables
//
// check if type exists in the built-in table, then do a look-up to see if the operation
// is supported

void check_ast(Ast* ast, AstContext* context) {
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

    for
      vector_iter(idx, ast->module_vec) {
        Module* module = vector_get(&ast->module_vec, idx);
        check_module(module, context);
      }

    if (context->is_lib && found_main) {
      panic("Missing program entry point 'main'\n");
    }
}

void check_module(Module* module, AstContext* context) {
  /*
   * Check for redefinitions of symbols
   */
  GHashTable* symbol_set = g_hash_table_new(g_str_hash, g_str_equal);

    for
      vector_iter(idx, module->symbol_vec) {
        Symbol* symbol = vector_get(&module->symbol_vec, idx);
        if (g_hash_table_contains(symbol_set, symbol->name)) {
          panic("redefinition of symbol %s\n", symbol->name);
        }
      }
}

Type deduce_type(Expr* expr);
