#ifndef ELX_TOKEN_TABLES_H
#define ELX_TOKEN_TABLES_H

#include "token_kind.h"


typedef struct string_token {
    char* text;
    token_kind_t kind;
} string_token_t;

static const string_token_t operator_table[] = {
    { "?", TOK_QMARK },
    { "\"", TOK_DQUOTE },
    { "'", TOK_SQUOTE },
    { "!", TOK_EXCLAM },
    { ":", TOK_COLON },
    { "^", TOK_XOR },
    { "%", TOK_PERCENT },
    { "~", TOK_BINV },
    { "-", TOK_MINUS },
    { "+", TOK_PLUS },
    { "$", TOK_DOLLAR },
    { "`", TOK_TICK },
    { "*", TOK_STAR },
    { "/", TOK_FSLASH },
    { "\\", TOK_BSLASH },
    { "{", TOK_LBRACE },
    { "}", TOK_RBRACE },
    { "@", TOK_AT },
    { "=", TOK_EQ },
    { "(", TOK_LPAREN },
    { ")", TOK_RPAREN },
    { "#", TOK_POUND },
    { "&", TOK_AMPER },
    { "[", TOK_LBRACK },
    { "]", TOK_RBRACK },
    { "|", TOK_PIPE },
    { ">", TOK_GT },
    { "<", TOK_LT },
    { ";", TOK_SEMICOLON },
    { "||", TOK_OR },
    { "&&", TOK_AND },
    { "<<", TOK_SHL },
    { ">>", TOK_SHR },
    { ">=", TOK_GTE },
    { "<=", TOK_LTE },
    { "->", TOK_ARROW },
    { "==", TOK_EQEQ },
    { "!=", TOK_NOTEQ },
    { "+=", TOK_PLUSEQ },
    { "-=", TOK_MINEQ },
    { "*=", TOK_TIMESEQ },
    { "/=", TOK_DIVEQ },
    { "~=", TOK_BINVEQ },
    { "&=", TOK_BANDEQ },
    { "^=", TOK_BXOREQ },
    { "%=", TOK_MODEQ },
    { "<<=", TOK_SHLEQ },
    { ">>=", TOK_SHREQ },
};

static const string_token_t keyword_table[] = {
    { "as", TOK_KW_AS },
    { "async", TOK_KW_ASYNC },
    { "await", TOK_KW_AWAIT },
    { "break", TOK_KW_BREAK },
    { "const", TOK_KW_CONST },
    { "continue", TOK_KW_CONTINUE },
    { "else", TOK_KW_ELSE },
    { "enum", TOK_KW_ENUM },
    { "false", TOK_KW_FALSE },
    { "fn", TOK_KW_FN },
    { "for", TOK_KW_FOR },
    { "if", TOK_KW_IF },
    { "impl", TOK_KW_IMPL },
    { "in", TOK_KW_IN },
    { "let", TOK_KW_LET },
    { "match", TOK_KW_MATCH },
    { "module", TOK_KW_MODULE },
    { "mut", TOK_KW_MUT },
    { "pub", TOK_KW_PUB },
    { "ref", TOK_KW_REF },
    { "return", TOK_KW_RETURN },
    { "self", TOK_KW_SELF },
    { "Self", TOK_KW_SELFTYPE },
    { "static", TOK_KW_STATIC },
    { "struct", TOK_KW_STRUCT },
    { "super", TOK_KW_SUPER },
    { "trait", TOK_KW_TRAIT },
    { "true", TOK_KW_TRUE },
    { "type", TOK_KW_TYPE },
    { "use", TOK_KW_USE },
    { "while", TOK_KW_WHILE },
};

#endif

