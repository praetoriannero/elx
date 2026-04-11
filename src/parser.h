#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lexer.h"
#include "vector.h"

struct Expr;
typedef struct Expr Expr;

typedef struct {
  Vector stmts;
} Body;

typedef enum {
  TYPE_KIND_RAW,
  TYPE_KIND_PTR,
  TYPE_KIND_REF,
} TypeKind;

typedef struct type_ {
  TypeKind kind;
  Vector path;
  char* name;
} Type;

typedef struct {
  char* name;
  Type type;
} StructField;

typedef enum {
  MEMBER_KIND_DATA,
  MEMBER_KIND_FUNCTION,
} MemberKind;

typedef struct {
  char* name;
  Vector param_vec; // vec<func_arg_t>
  Body body;
  Type ret_type;
} Func;

typedef struct {
  MemberKind member_kind;
  union {
    StructField struct_field;
    Func struct_func;
  };
} Member;

typedef struct {
  char* name;
  Vector field_vec;  // vec<struct_field_t>
  Vector method_vec; // vec<funct_t>
} Struct;

typedef struct {
  char* name;
  Type type;
} FuncArg;

typedef enum {
  LITERAL_KIND_STRING,
  LITERAL_KIND_CHAR,
  LITERAL_KIND_INTEGER,
  LITERAL_KIND_FLOAT,
  LITERAL_KIND_BYTE,
  LITERAL_KIND_BOOL,
} LiteralKind;

typedef struct {
  LiteralKind kind;
  union {
    char* string;
    char* char_;
    char* integer;
    char* float_;
    char* byte;
    char* bool_;
  };
} Literal;

typedef enum {
  // EmptyExpression
  EXPR_KIND_EMPTY,

  // LiteralExpression
  EXPR_KIND_LITERAL,

  // PathExpression
  EXPR_KIND_PATH,

  // OperatorExpression
  EXPR_KIND_BINARY,
  EXPR_KIND_UNARY,

  // GroupedExpression
  EXPR_KIND_GROUP,

  // ArrayExpression
  EXPR_KIND_ARRAY_EXPLICIT,
  EXPR_KIND_ARRAY_IMPLICIT,

  // IndexExpression
  EXPR_KIND_ARRAY_INDEX,

  // TupleExpression
  EXPR_KIND_TUPLE,

  // note: this is really a field expression
  // TupleIndexingExpression
  // EXPR_KIND_TUPLE_INDEX,

  // StructExpression
  EXPR_KIND_STRUCT,

  // CallExpression
  EXPR_KIND_CALL,

  // MethodCallExpression
  EXPR_KIND_METHOD_CALL,

  // FieldExpression
  EXPR_KIND_FIELD,

  // ClosureExpression
  EXPR_KIND_CLOSURE,

  // RangeExpression
  EXPR_KIND_RANGE,

  // AssignmentExpression
  EXPR_KIND_ASSIGN,

  // future
  // AwaitExpression
  // AsyncBlockExpression
  // UnderscoreExpression

  // JumpExpression
  // EXPR_KIND_RETURN,
  // EXPR_KIND_BREAK,
  // EXPR_KIND_CONTINUE,

  EXPR_KIND_IDENT,
} ExprKind;

typedef enum {
  // return break continue yield
  EXPR_PREC_JUMP = 10,

  // = += -= *= /= %= &= |= ^= <<= >>=
  EXPR_PREC_ASSIGN = 20,

  // .. ..=
  EXPR_PREC_RANGE = 30,

  // ||
  EXPR_PREC_LOR = 40,

  // &&
  EXPR_PREC_LAND = 50,

  // == != >= <= > <
  EXPR_PREC_COMP = 60,

  // |
  EXPR_PREC_BOR = 70,

  // ^
  EXPR_PREC_BXOR = 80,

  // &
  EXPR_PREC_BAND = 90,

  // >> <<
  EXPR_PREC_SHIFT = 100,

  // + -
  EXPR_PREC_ADDSUB = 110,

  // % * /
  EXPR_PREC_MULDIV = 120,

  // unary + - ^ !
  EXPR_PREC_PREFIX = 130,

  // function calls, array indexing, field expressions, method calls
  EXPR_PREC_POSTFIX = 140,

  // paths identifiers
  EXPR_PREC_UNAMBIGUOUS = 250,
} ExprPrecedence;

ExprPrecedence get_precedence(const ExprKind expr_kind, const char* operator);

struct Expr {
  ExprKind expr_kind;

  union {
    struct {
      bool mut;
      Expr* expr;
    } assign_expr;

    struct {
      Expr* lhs;
      Expr* rhs;
      Token op;
    } binary_expr;

    struct {
      Expr* inner;
      Token op;
    } unary_expr;

    struct {
      Expr* kind;
      Expr* size;
    } array_implicit_expr;

    struct {
      Vector arg_vec;
    } array_explicit_expr;

    struct {
      Expr* object;
      Vector arg_vec;
    } struct_init_expr;

    struct {
      Expr* object;
      Vector arg_vec; // vec<Expr>
    } call_expr;

    struct {
      Expr* object;
      char* name;
    } field_expr;

    struct {
      Expr* object;
      Expr* index;
    } array_index_expr;

    struct {
      char* stem;
      Expr* expr;
    } path_expr;

    struct {
      Expr* object;
      char* method;
      Vector arg_vec; // vec<Expr>
    } method_call_expr;

    char* ident_expr;

    Literal literal_expr;

    // struct {
    //     Expr* expr;
    // } break_expr;
    //
    // struct {
    //     Expr* expr;
    // } return_expr;
    //
    // struct {
    //     Expr* expr;
    // } continue_expr;
  };
};

typedef struct {
  Vector tokens;
} ExprStream;

typedef struct {
  Expr* expr;
} ExprStmt;

typedef enum {
  ASSIGN_OP_KIND_PLUS_EQUAL,
  ASSIGN_OP_KIND_MINUS_EQUAL,
  ASSIGN_OP_KIND_TIMES_EQUAL,
  ASSIGN_OP_KIND_DIVIDE_EQUAL,
  ASSIGN_OP_KIND_MOD_EQUAL,
  ASSIGN_OP_KIND_SHL_EQUAL,
  ASSIGN_OP_KIND_SHR_EQUAL,
  ASSIGN_OP_KIND_AND_EQUAL,
  ASSIGN_OP_KIND_OR_EQUAL,
  ASSIGN_OP_KIND_XOR_EQUAL,
  ASSIGN_OP_KIND_EQUAL,
} AssignOpKind;

typedef struct {
  char* name;
  bool mut;
  Expr* expr;
} AssignStmt;

typedef struct {
  Expr* expr;
} ReturnStmt;

typedef struct {
  char* iterator;
  Expr* iterable;
  Body body;
} ForStmt;

typedef struct {
  Expr* condition;
  Body body;
} WhileStmt;

typedef struct {
  Body body;
} ElseClause;

typedef struct {
  Expr* condition;
  Body body;
} ElifClause;

typedef struct {
  Expr* condition;
  Body body;
  Vector elif_clause_vec;
  ElseClause else_clause;
} IfStmt;

typedef struct {
  char* ident;
} BreakStmt;

typedef struct {
  char* ident;
} ContinueStmt;

struct symbol;

typedef struct {
  char* name;
  Vector symbol_vec; // vec<symbol_t>
} Module;

typedef enum {
  STMT_KIND_UNDEFINED,
  STMT_KIND_ASSIGN,
  STMT_KIND_RETURN,
  STMT_KIND_EXPR,
  STMT_KIND_FOR,
  STMT_KIND_WHILE,
  STMT_KIND_IF,
  STMT_KIND_BREAK,
  STMT_KIND_CONTINUE,
} StmtKind;

typedef struct {
  StmtKind kind;
  union {
    AssignStmt assign_stmt;
    ReturnStmt return_stmt;
    ExprStmt expr_stmt;
    ForStmt for_stmt;
    WhileStmt while_stmt;
    IfStmt if_stmt;
    BreakStmt break_stmt;
    ContinueStmt cont_stmt;
  };
} Stmt;

typedef struct {
  char* name;
  bool mut;
  Type type;
  Expr* expr;
} Global;

typedef enum {
  ENUM_VARIANT_NOM,
  ENUM_VARIANT_ALG,
} EnumVariant;

typedef struct {
  char* name;
  Type type;
  EnumVariant variant;
} EnumKind;

typedef struct {
  char* name;
  Vector kind_vec; // vec<enum_kind_t>
} Enum;

typedef struct {
  Vector path;
} Import;

typedef enum {
  SYMBOL_KIND_UNDEFINED,
  SYMBOL_KIND_STRUCT,
  SYMBOL_KIND_MODULE,
  SYMBOL_KIND_FUNC,
  SYMBOL_KIND_GLOBAL,
  SYMBOL_KIND_ENUM,
} SymbolKind;

typedef struct {
  Vector path;
  char* name;

  SymbolKind kind;
  union {
    Struct struct_case;
    Module module_case;
    Func func_case;
    Global global_case;
    Enum enum_case;
  };
} Symbol;

typedef struct {
  Vector module_vec; // vec<module_t>
} Ast;

typedef struct parser {
  Lexer lexer;
} Parser;

Ast parser_parse(Arena* arena, Parser* self);

Parser* parser_new(Arena* arena, Lexer lexer);

void parser_init(Parser* self, Lexer lexer);

Type parse_type(Arena* arena, Parser* self);

Body visit_body(Arena* arena, Parser* self);

Symbol visit_struct(Arena* arena, Parser* self);

Symbol visit_module(Arena* arena, Parser* self);

Symbol visit_global(Arena* arena, Parser* self, bool is_var);

Symbol visit_enum(Arena* arena, Parser* self);

Symbol visit_import(Arena* arena, Parser* self);

Symbol visit_func(Arena* arena, Parser* self);

Stmt visit_expr_stmt(Arena* arena, Parser* self);

Expr* visit_expr(Arena* arena, Parser* self, TokenKind stop_token);

u64 count_csv(const char* data, char sep);

void print_ast(Ast* ast);
