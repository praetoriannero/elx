#include "analyzer.h"
#include "parser.h"

void check_main(func_t* main_func);

// idea: table of trait impls for built-ins just like we did with the operator lookup tables
//
// check if type exists in the built-in table, then do a look-up to see if the operation
// is supported


void check_ast(ast_t* ast, context_t* context) {
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

    for vector_iter(ast->module_vec, idx) {
        module_t* module = vector_get(&ast->module_vec, idx);
        check_module(module, context);
    }

    if (context->is_lib && found_main) {
        panic("Missing program entry point 'main'\n");
    }
}

void check_module(module_t* module, context_t* context) {
    /*
     * Check for redefinitions of symbols
     */

    for vector_iter(module->symbol_vec, idx) {
        symbol_t* symbol = vector_get(&module->symbol_vec, idx);
    }
}

type_t deduce_type(expr_t* expr);
