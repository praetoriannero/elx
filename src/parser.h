#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "lexer.h"
#include "str_utils.h"
#include "vector.h"

struct expr_t;
typedef struct expr_t expr_t;

typedef struct {
    vector_t stmts;
} body_t;

typedef enum {
    TYPE_KIND_RAW,
    TYPE_KIND_PTR,
    TYPE_KIND_REF,
} type_kind_t;

// typedef struct {
//     char* name;
//     vector_t path;
// } ident_t;

typedef struct type_ {
    type_kind_t kind;
    vector_t path;
    char* name;
} type_t;

typedef struct {
    char* name;
    type_t type;
} struct_field_t;

typedef enum {
    MEMBER_KIND_DATA,
    MEMBER_KIND_FUNCTION,
} member_kind_t;

typedef struct {
    char* name;
    vector_t arg_vec; // vec<func_arg_t>
    body_t body;
    type_t ret_type;
} func_t;

typedef struct {
    member_kind_t member_kind;
    union {
        struct_field_t struct_field;
        func_t struct_func;
    };
} member_t;

typedef struct {
    char* name;
    vector_t field_vec;  // vec<struct_field_t>
    vector_t method_vec; // vec<funct_t>
} struct_t;

typedef struct {
    char* name;
    type_t type;
} func_arg_t;

typedef enum {
    LITERAL_KIND_STRING,
    LITERAL_KIND_CHAR,
    LITERAL_KIND_INTEGER,
    LITERAL_KIND_FLOAT,
    LITERAL_KIND_BYTE,
    LITERAL_KIND_BOOL,
} literal_kind_t;

typedef struct {
    literal_kind_t kind;
    union {
        char* string;
        char* char_;
        char* integer;
        char* float_;
        char* byte;
        char* bool_;
    };
} literal_t;

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
} expr_kind_t;

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
} expr_precedence_t;

expr_precedence_t get_precedence(const expr_kind_t expr_kind, const char* operator);

struct expr_t {
    expr_kind_t expr_kind;

    union {
        struct {
            bool mut;
            expr_t* expr;
        } assign_expr;

        struct {
            expr_t* lhs;
            expr_t* rhs;
            token_t op;
        } binary_expr;

        struct {
            expr_t* inner;
            token_t op;
        } unary_expr;

        struct {
            expr_t* kind;
            expr_t* size;
        } array_implicit_expr;

        struct {
            vector_t arg_vec;
        } array_explicit_expr;

        struct {
            expr_t* object;
            vector_t arg_vec;
        } struct_init_expr;

        struct {
            expr_t* object;
            vector_t arg_vec; // vec<expr_t>
        } call_expr;

        struct {
            expr_t* object;
            char* name;
        } field_expr;

        struct {
            expr_t* object;
            expr_t* index;
        } array_index_expr;

        struct {
            char* stem;
            expr_t* expr;
        } path_expr;

        struct {
            expr_t* object;
            char* method;
            vector_t arg_vec; // vec<expr_t>
        } method_call_expr;

        char* ident_expr;

        literal_t literal_expr;

        // struct {
        //     expr_t* expr;
        // } break_expr;
        //
        // struct {
        //     expr_t* expr;
        // } return_expr;
        //
        // struct {
        //     expr_t* expr;
        // } continue_expr;
    };
};

typedef struct {
    vector_t tokens;
} expr_stream_t;

typedef struct {
    expr_t* expr;
} expr_stmt_t;

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
} assign_op_kind_t;

typedef struct {
    char* name;
    bool mut;
    // assign_op_kind_t assign_op;
    expr_t* expr;
} assign_stmt_t;

// typedef struct {
//     char* name;
//     expr_t* expr;
// } let_stmt_t;

typedef struct {
    expr_t* expr;
} return_stmt_t;

typedef struct {
    char* iterator;
    expr_t* iterable;
    body_t body;
} for_stmt_t;

typedef struct {
    expr_t* condition;
    body_t body;
} while_stmt_t;

typedef struct {
    body_t body;
} else_clause_t;

typedef struct {
    expr_t* condition;
    body_t body;
} elif_clause_t;

typedef struct {
    expr_t* condition;
    body_t body;
    vector_t elif_clause_vec;
    else_clause_t else_clause;
} if_stmt_t;

typedef struct {
    char* ident;
} break_stmt_t;

typedef struct {
    char* ident;
} continue_stmt_t;

struct symbol;

typedef struct {
    char* name;
    vector_t symbol_vec; // vec<symbol_t>
} module_t;

typedef enum {
    STMT_KIND_UNDEFINED,
    STMT_KIND_ASSIGN,
    // STMT_KIND_LET,
    STMT_KIND_RETURN,
    STMT_KIND_EXPR,
    STMT_KIND_FOR,
    STMT_KIND_WHILE,
    STMT_KIND_IF,
    STMT_KIND_BREAK,
    STMT_KIND_CONTINUE,
} stmt_kind_t;

typedef struct {
    stmt_kind_t kind;
    union {
        assign_stmt_t assign_stmt;
        // let_stmt_t let_stmt;
        return_stmt_t return_stmt;
        expr_stmt_t expr_stmt;
        for_stmt_t for_stmt;
        while_stmt_t while_stmt;
        if_stmt_t if_stmt;
        break_stmt_t break_stmt;
        continue_stmt_t cont_stmt;
    };
} stmt_t;

typedef struct {
    char* name;
    bool mut;
    type_t type;
    expr_t* expr;
} global_t;

typedef enum {
    ENUM_VARIANT_NOM,
    ENUM_VARIANT_ALG,
} enum_variant_t;

typedef struct {
    char* name;
    type_t type;
    enum_variant_t variant;
} enum_kind_t;

typedef struct {
    char* name;
    vector_t kind_vec; // vec<enum_kind_t>
} enum_t;

// typedef struct {
//     char* ident;
//     vector_t fn_vec;
// } impl_t;

typedef struct {
    vector_t path;
} import_t;

typedef enum {
    SYMBOL_KIND_UNDEFINED,
    SYMBOL_KIND_STRUCT,
    SYMBOL_KIND_MODULE,
    SYMBOL_KIND_FUNC,
    SYMBOL_KIND_GLOBAL,
    SYMBOL_KIND_ENUM,
    SYMBOL_KIND_IMPORT,
    // SYMBOL_KIND_IMPL,
} symbol_kind_t;

typedef struct {
    vector_t path;
    symbol_kind_t kind;
    union {
        struct_t struct_case;
        module_t module_case;
        func_t func_case;
        global_t global_case;
        enum_t enum_case;
        import_t import_case;
        // impl_t impl_case;
    };
} symbol_t;

typedef struct {
    vector_t module_vec; // vec<module_t>
} ast_t;

typedef struct parser {
    lexer_t lexer;
} parser_t;

ast_t parser_parse(arena_t* arena, parser_t* self);

parser_t* parser_new(arena_t* arena, lexer_t lexer);

void parser_init(parser_t* self, lexer_t lexer);

type_t parse_type(arena_t* arena, parser_t self);

body_t visit_body(arena_t* arena, parser_t* self);

symbol_t visit_struct(arena_t* arena, parser_t* self);

symbol_t visit_module(arena_t* arena, parser_t* self);

symbol_t visit_global(arena_t* arena, parser_t* self, bool is_var);

symbol_t visit_enum(arena_t* arena, parser_t* self);

symbol_t visit_import(arena_t* arena, parser_t* self);

symbol_t visit_func(arena_t* arena, parser_t* self);

stmt_t visit_expr_stmt(arena_t* arena, parser_t* self);

expr_t* visit_expr(arena_t* arena, parser_t* self, token_kind_t stop_token);

u64 count_csv(const char* data, char sep);

void print_ast(ast_t* ast);
