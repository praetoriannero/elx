/**
 * @file analyzer.h
 * @brief Semantic analysis for the ELX compiler.
 *
 * Performs scope resolution, symbol table management, and AST traversal
 * to validate the program's semantics before code generation.
 */

#pragma once

#include "core/allocator.h"
#include "core/hashtable.h"
#include "parser/parser.h"

/**
 * @brief A named scope containing symbols and optional child scopes.
 *
 * Represents a lexical scope in the ELX language, containing a hash table
 * of visible symbols and optionally child scopes for nested blocks.
 */
typedef struct {
  HashTable symbol_table;     // &HashTable<char*, Symbol*>
  Vector child_scopes;        // Vector<Scope>
  struct Scope* parent_scope; // null if global scope
} Scope;

/*  @brief Push a new scope onto the end of the linked list
 *  @param alloc The memory allocator
 *  @param scope The parent scope to add to
 *  @return The new scope we just added
 */
Scope* scope_push(Allocator* alloc, Scope* scope);

/*  @brief AnalyzerContext contains the entry point of the
 *    scope linked list as the global scope, and it also
 *    contains the current scope, local_scope.
 */
typedef struct {
  Scope* global_scope;
  Scope* local_scope;
} AnalyzerContext;

/* @brief Initialize the analyzer's context
 * @param ctx The context to initialize
 */
void analyzer_context_init(AnalyzerContext* ctx);

/*  @brief Analyzer visitor functions
 */
void analyzer_visit_expr(Expr* expr, AnalyzerContext* ctx);
void analyzer_visit_assign_stmt(AssignStmt* node, AnalyzerContext* ctx);
void analyzer_visit_continue_stmt(ContinueStmt* node, AnalyzerContext* ctx);
void analyzer_visit_expr_stmt(ExprStmt* node, AnalyzerContext* ctx);
void analyzer_visit_for_stmt(ForStmt* node, AnalyzerContext* ctx);
void analyzer_visit_if_stmt(IfStmt* node, AnalyzerContext* ctx);
void analyzer_visit_return_stmt(ReturnStmt* node, AnalyzerContext* ctx);
void analyzer_visit_while_stmt(WhileStmt* node, AnalyzerContext* ctx);
void analyzer_visit_body(Body* node, AnalyzerContext* ctx);
void analyzer_visit_enum(Enum* node, AnalyzerContext* ctx);
void analyzer_visit_expr(Expr* node, AnalyzerContext* ctx);
void analyzer_visit_func(Func* node, AnalyzerContext* ctx);
void analyzer_visit_global(Global* node, AnalyzerContext* ctx);
void analyzer_visit_import(Import* node, AnalyzerContext* ctx);
void analyzer_visit_module(Module* module, AnalyzerContext* ctx);
void analyzer_visit_struct(Struct* node, AnalyzerContext* ctx);
void analyzer_visit_ast(Ast* node, AnalyzerContext* ctx);
