#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "token.h"

typedef struct elx_path {
  char** paths;
  uint32_t path_count;
} elx_path_t;

typedef struct elx_ident {
  char* name;
  elx_path_t path;
} elx_ident_t;

typedef struct elx_struct_field {
  char* name;
  elx_ident_t type;
} elx_StructField;

typedef struct elx_struct {
  elx_ident_t ident;
  elx_StructField* fields;
  uint32_t field_count;
} elx_Struct;

typedef struct elx_func_arg {
  char* name;
  elx_ident_t type;
} elx_FuncArg;

typedef struct elx_expr {
  Token* tokens;
  uint32_t token_count;
} elx_Expr;

typedef struct elx_expr_stmt {
  elx_Expr expr;
} elx_ExprStmt;

typedef enum elx_assign_op_kind {
  ELX_PLUS_EQUAL,
  ELX_MINUS_EQUAL,
  ELX_TIMES_EQUAL,
  ELX_DIVIDE_EQUAL,
  ELX_MOD_EQUAL,
  ELX_SHL_EQUAL,
  ELX_SHR_EQUAL,
  ELX_AND_EQUAL,
  ELX_OR_EQUAL,
  ELX_XOR_EQUAL,
  ELX_EQUAL,
} elx_AssignOpKind;

typedef struct elx_assign_stmt {
  char* name;
  elx_AssignOpKind assign_op;
  elx_Expr expr;
} elx_AssignStmt;

// typedef struct elx_variable {
//     elx_ident_t ident;
//     bool mut;
//     elx_ident_t type;
// } elx_variable_t;

struct elx_stmt;

typedef struct {
  struct elx_stmt* stmts;
  uint32_t stmt_count;
} elx_Body;

typedef struct {
  char* name;
  elx_Expr expr;
} elx_let_Stmt;

typedef struct {
  char* name;
  elx_Expr expr;
} elx_var_Stmt;

typedef struct {
  elx_Expr expr;
} elx_ReturnStmt;

typedef struct {
  char* iterator;
  elx_Expr iterable;
  elx_Body body;
} elx_ForStmt;

typedef struct {
  elx_Expr condition;
  elx_Body body;
} elx_WhileStmt;

typedef struct {
  elx_Body body;
} elx_ElseClause;

typedef struct {
  elx_Expr condition;
  elx_Body body;
} elx_ElifClause;

typedef struct {
  elx_Expr condition;
  elx_Body body;
  elx_ElifClause* elif_clauses;
  uint32_t elif_clause_count;
  elx_ElseClause else_clause;
} elx_IfStmt;

typedef struct {
  elx_ident_t* ident;
} elx_BreakStmt;

typedef struct {
  elx_ident_t* ident;
} elx_ContinueStmt;

struct elx_symbol;

typedef struct {
  elx_ident_t ident;
  struct elx_symbol* symbols;
  uint32_t symbol_count;
} elx_module_stmt;

typedef enum {
  ELX_ASSIGN_STMT,
  ELX_LET_STMT,
  ELX_VAR_STMT,
  ELX_RETURN_STMT,
  ELX_EXPR_STMT,
  ELX_FOR_STMT,
  ELX_WHILE_STMT,
  ELX_IF_STMT,
  ELX_BREAK_STMT,
  ELX_CONTINUE_STMT,
  ELX_MODULE_STMT,
} elx_StmtKind;

typedef union {
  elx_ExprStmt expr_stmt;
  elx_AssignStmt assign_stmt;
} elx_stmt_variant_t;

typedef struct {
  elx_StmtKind kind;
  elx_stmt_variant_t variant;
} elx_Stmt;

typedef struct {
  elx_ident_t ident;
  elx_FuncArg* args;
  uint32_t arg_count;
  elx_Body body;
} elx_Func;

typedef enum {
  ELX_STRUCT_KIND,
  ELX_GLOBAL_KIND,
  ELX_ENUM_KIND,
  ELX_MODULE_KIND,
  ELX_FUNC_KIND,
  ELX_IMPORT_KIND,
} elx_SymbolKind;

typedef union {
  elx_Struct struct_kind;
} elx_symbol_variant_t;

typedef struct {
  elx_SymbolKind kind;
  elx_symbol_variant_t variant;
} elx_Symbol;

typedef struct {
  elx_ident_t ident;
} elx_Module;
