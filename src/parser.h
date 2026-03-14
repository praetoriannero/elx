#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "lexer.h"
#include "str_utils.h"
#include "vector.h"

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
    vector_t field_vec; // vec<struct_field_t>
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
} literal_kind_t;

typedef struct {
    literal_kind_t kind;
    union {
        char* string;
        char* char_;
        char* integer;
        char* float_;
        char* byte;
    };
} literal_t;

typedef enum {
    //   LiteralExpression
    //   PathExpression
    //   OperatorExpression
    //   GroupedExpression
    //   ArrayExpression
    //   AwaitExpression
    //   IndexExpression
    //   TupleExpression
    //   TupleIndexingExpression
    //   StructExpression
    //   CallExpression
    //   MethodCallExpression
    //   FieldExpression
    //   ClosureExpression
    //   AsyncBlockExpression
    //   ContinueExpression
    //   BreakExpression
    //   RangeExpression
    //   ReturnExpression
    //   UnderscoreExpression

    EXPR_KIND_BINARY,
    EXPR_KIND_UNARY,
    // EXPR_KIND_GROUP,
    EXPR_KIND_ARRAY,
    EXPR_KIND_STRUCT,
    EXPR_KIND_METHOD_CALL,
    EXPR_KIND_CALL,
    EXPR_KIND_FIELD,
    EXPR_KIND_PATH,
    EXPR_KIND_ARRAY_INDEX,
    EXPR_KIND_TUPLE_INDEX,
    EXPR_KIND_IDENT,
    EXPR_KIND_AS,
    EXPR_KIND_LITERAL,

} expr_kind_t;

typedef struct expr_t expr_t;

typedef enum {
    PREC_LOR,
    PREC_LAND,
    PREC_COMP,
    PREC_BOR,
    PREC_BXOR,
    PREC_BAND,
    PREC_SHIFT,
    PREC_ADDSUB,
    PREC_MULDIV,
    PREC_CAST,
    PREC_UNARY,
    PREC_CALL,
    PREC_STRUCT,
    PREC_PATH,
} precedence_t;

static precedence_t get_precedence(const expr_kind_t expr_kind, const char* operator) {
    switch (expr_kind) {
    case EXPR_KIND_BINARY:
        if (streq("||", operator)) {
            return PREC_LOR;
        } else if (streq("&&", operator)) {
            return PREC_LAND;
        } else if (streq("==", operator) || streq("!=", operator) ||
                   streq("<", operator) || streq(">", operator) ||
                   streq("<=", operator) || streq(">=", operator)) {
            return PREC_COMP;
        } else if (streq("|", operator)) {
            return PREC_BOR;
        } else if (streq("&", operator)) {
            return PREC_BAND;
        } else if (streq(">>", operator) || streq(">>", operator)) {
            return PREC_SHIFT;
        } else if (streq("+", operator) || streq("-", operator)) {
            return PREC_ADDSUB;
        } else if (streq("*", operator) || streq("/", operator)) {
            return PREC_MULDIV;
        }
    case EXPR_KIND_UNARY:
        return PREC_UNARY;
    case EXPR_KIND_AS:
        return PREC_CAST;
    case EXPR_KIND_STRUCT:
        return PREC_STRUCT;
    }
}

struct expr_t {
    expr_kind_t expr_kind;

    union {
        struct {
            expr_t* lhs;
            token_t op;
            expr_t* rhs;
        } binary_op;

        struct {
            expr_t* inner;
            token_t op;
        } unary_op;

        // expr_t* group;

        struct {
            expr_t* kind;
            expr_t* size;
        } array;

        struct {
            vector_t field_init_vec;
        } struct_;

        struct {
            expr_t* object;
            vector_t arg_vec; // vec<expr_t>
        } call;

        struct {
            expr_t* object;
            char* name;
        } field;

        struct {
            expr_t* object;
            expr_t* index;
        } array_index;

        // vector_t path;  // vec<char*>
        struct {
            vector_t path;
        } path_expr;

        char* identifier;

        struct {
            char* name;
            type_t type;
        } object;

        literal_t literal;
    };
};

typedef struct {
    vector_t tokens;
} expr_stream_t;

typedef struct {
    expr_stream_t expr;
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
    assign_op_kind_t assign_op;
    expr_stream_t expr;
} assign_stmt_t;

typedef struct {
    char* name;
    expr_stream_t expr;
} let_stmt_t;

typedef struct {
    expr_stream_t expr;
} return_stmt_t;

typedef struct {
    char* iterator;
    expr_stream_t iterable;
    body_t body;
} for_stmt_t;

typedef struct {
    expr_stream_t condition;
    body_t body;
} while_stmt_t;

typedef struct {
    body_t body;
} else_clause_t;

typedef struct {
    expr_stream_t condition;
    body_t body;
} elif_clause_t;

typedef struct {
    expr_stream_t condition;
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
    vector_t symbol_vec;
} module_t;

typedef enum {
    STMT_KIND_UNDEFINED,
    STMT_KIND_ASSIGN,
    STMT_KIND_LET,
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
        let_stmt_t let_stmt;
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
    expr_stream_t expr;
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
    vector_t module_vec;
} ast_t;

typedef struct parser {
    lexer_t lexer;
} parser_t;

ast_t parser_parse(arena_t* arena, parser_t* self);

parser_t* parser_new(arena_t* arena, lexer_t lexer);

void parser_init(parser_t* self, lexer_t lexer);
