/*  HIR lowering performs the following:
 *  - desugaring
 *    -- convert lhs ?= rhs to lhs = lhs ? rhs
 *    -- convert nested operations to intermediate products
 *  - type checking
 *    -- ensure all types passed into functions and assignments are correct
 */
#pragma once

#include "core/hashtable.h"
#include "parser/parser.h"

typedef struct {
  Ast* ast;
  Allocator* alloc;
  HashTable symbol_table; // HashTable<char*, Box<Module*>>
} HirContext;

typedef struct {
  Vector param_vec;
  Span name;
  u64 id;
} HirType;

typedef enum {
  HIR_NODE_SYMBOL,
  HIR_NODE_VARDECL,
  HIR_NODE_EXPR,
  HIR_NODE_BRANCH,
  HIR_NODE_CALL,
  HIR_NODE_CTOR,
  HIR_NODE_BODY,
  HIR_NODE_PARAM,
} HirNodeKind;

typedef enum {
  HIR_EXPR_KIND_INVALID,
  HIR_EXPR_KIND_LITERAL,
  HIR_EXPR_KIND_ARRAY,
  HIR_EXPR_KIND_INDEX,
  // HIR_EXPR_KIND_ENUM,
  HIR_EXPR_KIND_STRUCT,
  HIR_EXPR_KIND_CALL,
  HIR_EXPR_KIND_FIELD,
  // HIR_EXPR_KIND_CLOSURE,
  HIR_EXPR_KIND_ASSIGN,
  HIR_EXPR_KIND_IDENT,
} HirExprKind;

struct HirExpr;
typedef struct HirExpr HirExpr;

struct HirExpr {
  HirExprKind expr_kind;
  Span span;

  union {
    struct {
      LiteralKind lit_kind;
      Literal lit_variant;
    } lit_expr;

    struct {
      HirExpr* kind;
      HirExpr* count;
      Vector arg_vec;
    } array_expr;

    struct {
      HirExpr* object;
      HirExpr* index;
    } index_expr;

    struct {
      HirType type;
      Vector arg_vec;
    } struct_expr;

    struct {
      HirExpr* object;
      Vector arg_vec;
    } call_expr;

    struct {
      HirExpr* object;
      StructField field;
    } field_expr;

    struct {
      HirExpr* lhs;
      HirExpr* rhs;
    } assign_expr;

    struct {
      String name;
      u64 id;
    } ident_expr;
  };
};


/*  Carry over bodies but simplify statements
 */
typedef struct {
  Vector stmt_vec;
} HirBody;

typedef struct {
  Expr condition;
  String label;
  HirBody body;
} HirBranchNode;


/*  Expression statement
 */
typedef struct {
  
} HirStmtExpr;

typedef struct {
  String name;
  u64 id;
  Span span;
  HirExpr expr;
} HirStmtVarDecl;

/*  Assignment to a variable
 */
typedef struct {
  String name;
  Span span;
  HirExpr expr;
} HirStmtAssign;

/*  Desugar all
 *    - if, else-if, else
 *    - while
 *    - for
 *  into the same set of conditionals
 */
typedef struct {
  Vector branch_node_vec;
} HirStmtBranch;

typedef enum {
  HIR_STMT_KIND_UNDEFINED,
  HIR_STMT_KIND_VARDECL,
  HIR_STMT_KIND_ASSIGN,
  HIR_STMT_KIND_EXPR,
  HIR_STMT_KIND_BRANCH,
} HirStmtKind;

/*  Statement types found within bodies
 */
typedef struct {
  HirStmtKind kind;
  union {
    HirStmtVarDecl var_decl_stmt;
    HirStmtAssign assign_stmt;
    HirStmtExpr expr_stmt;
    HirStmtBranch branch_stmt;
  }; 
} HirStmt;


typedef enum {
  HIR_SYMBOL_UNDEFINED,
  HIR_SYMBOL_TYPE,
  HIR_SYMBOL_FUNC,
  HIR_SYMBOL_ENUM,
  HIR_SYMBOL_GLOBAL,
  HIR_SYMBOL_IMPORT,
} HirSymbolKind;

/*  Parameter type for function, enum, and structs
 */
typedef struct {} HirParam;

typedef struct {
  Vector param_vec;
  u64 id;
  String name;
  HirType return_type;
} HirFunc;

typedef struct {
  HirType type;
  u64 enum_case;
} HirEnumVariant;

typedef struct {
  Vector case_vec;
  Vector case_values;
} HirEnum;

typedef struct {
  Span name;
  HirExpr expr;
} HirGlobal;

typedef struct {
  Span name;
  Vector path;
} HirImport;

typedef struct HirSymbol {
  HirSymbolKind kind;
  union {
    HirType hir_type;
    HirFunc hir_func;
    HirEnum hir_enum;
    HirImport hir_import;
    HirGlobal hir_global;
  };
} HirSymbol;

typedef struct {
  Vector hir_symbol_vec;
} HirRoot;

void hir_init(HirContext* self, Allocator* alloc, Ast* ast);

HirStmt hir_visit_expr(HirContext* self, TokenKind stop_token); 
HirStmt hir_visit_break_stmt(HirContext* self); 
HirStmt hir_visit_expr_stmt(HirContext* self); 
HirStmt hir_visit_for_stmt(HirContext* self); 
HirStmt hir_visit_if_stmt(HirContext* self); 
HirStmt hir_visit_while_stmt(HirContext* self); 
HirStmt hir_visit_return_stmt(HirContext* self); 
HirStmt hir_visit_continue_stmt(HirContext* self); 
HirStmt hir_visit_assign_stmt(HirContext* self); 
HirStmt hir_visit_body(HirContext* self); 
HirStmt hir_visit_struct(HirContext* self); 
HirStmt hir_visit_module(HirContext* self); 
HirStmt hir_visit_func(HirContext* self); 
HirStmt hir_visit_global(HirContext* self, bool is_var); 
HirStmt hir_visit_enum(HirContext* self); 
HirStmt hir_visit_import(HirContext* self); 
