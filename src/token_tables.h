#ifndef ELX_TOKEN_TABLES_H
#define ELX_TOKEN_TABLES_H

#include <stdlib.h>

#include "token_kind.h"

struct op_node {
    char text;
    token_kind_t kind;
    const struct op_node* children;
    size_t child_count;
};

typedef struct op_node op_node_t;

static const op_node_t shr_children[] = {
    {'=', TOK_SHREQ, NULL, 0},
};

static const op_node_t shl_children[] = {
    {'=', TOK_SHLEQ, NULL, 0},
};

static const op_node_t mod_children[] = {
    {'=', TOK_MODEQ, NULL, 0},
};

static const op_node_t xor_children[] = {
    {'=', TOK_XOREQ, NULL, 0},
};

static const op_node_t binv_children[] = {
    {'=', TOK_BINVEQ, NULL, 0},
};

static const op_node_t div_children[] = {
    {'=', TOK_DIVEQ, NULL, 0},
};

static const op_node_t times_children[] = {
    {'=', TOK_TIMESEQ, NULL, 0},
};

static const op_node_t min_children[] = {
    {'=', TOK_MINEQ, NULL, 0},
    {'>', TOK_ARROW, NULL, 0},
};

static const op_node_t plus_children[] = {
    {'=', TOK_PLUSEQ, NULL, 0},
};

static const op_node_t excl_children[] = {
    {'=', TOK_NOTEQ, NULL, 0},
};

static const op_node_t eq_children[] = {
    {'=', TOK_EQEQ, NULL, 0},
};

static const op_node_t gt_children[] = {
    {'>', TOK_SHR, shr_children, 1},
    {'=', TOK_GTE, NULL, 0},
};

static const op_node_t lt_children[] = {
    {'<', TOK_SHL, shl_children, 1},
    {'=', TOK_LTE, NULL, 0},
};

static const op_node_t and_children[] = {
    {'&', TOK_LAND, NULL, 0},
    {'=', TOK_ANDEQ, NULL, 0},
};

static const op_node_t or_children[] = {
    {'|', TOK_LOR, NULL, 0},
    {'=', TOK_OREQ, NULL, 0},
};

static const op_node_t fs_children[] = {
    {'/', TOK_COMMENT, NULL, 0},
    {'=', TOK_DIVEQ, NULL, 0},
};

static const op_node_t dec_children[] = {
    {'.', TOK_RANGE, NULL, 0},
};

static const op_node_t colon_children[] = {
    {':', TOK_PATH, NULL, 0},
};

static const op_node_t star_children[] = {
    {'=', TOK_TIMESEQ, NULL, 0},
};

static const op_node_t op_table[] = {
    {'.', TOK_DECIMAL, dec_children, 1},
    {'?', TOK_QMARK, NULL, 0},
    {'\"', TOK_DQUOTE, NULL, 0},
    {'\'', TOK_SQUOTE, NULL, 0},
    {'!', TOK_EXCLAM, excl_children, 1},
    {':', TOK_COLON, colon_children, 1},
    {'^', TOK_XOR, xor_children, 1},
    {'%', TOK_PERCENT, mod_children, 1},
    {'~', TOK_BINV, binv_children, 1},
    {'-', TOK_MINUS, min_children, 2},
    {'+', TOK_PLUS, plus_children, 1},
    {'$', TOK_DOLLAR, NULL, 0},
    {'*', TOK_STAR, star_children, 1},
    {'/', TOK_FSLASH, fs_children, 2},
    {'@', TOK_AT, NULL, 0},
    {'|', TOK_PIPE, or_children, 2},
    {'&', TOK_AMPER, and_children, 2},
    {'#', TOK_POUND, NULL, 0},
    {'=', TOK_EQ, eq_children, 1},
    {'>', TOK_GT, gt_children, 2},
    {'<', TOK_LT, lt_children, 2},
    // should these separators not be in the table?
    // separators/puncuators
    {',', TOK_COMMA, NULL, 0},
    {'[', TOK_LBRACK, NULL, 0},
    {']', TOK_RBRACK, NULL, 0},
    {'(', TOK_LPAREN, NULL, 0},
    {')', TOK_RPAREN, NULL, 0},
    {'{', TOK_LBRACE, NULL, 0},
    {'}', TOK_RBRACE, NULL, 0},
    {';', TOK_SEMICOLON, NULL, 0},
    // possibly drop this
    {'`', TOK_TICK, NULL, 0},
};

typedef struct string_token {
    char* text;
    token_kind_t kind;
} string_token_t;

static const string_token_t keyword_kind_table[] = {
    {"as", TOK_KW_AS},         {"async", TOK_KW_ASYNC},
    {"await", TOK_KW_AWAIT},   {"break", TOK_KW_BREAK},
    {"const", TOK_KW_CONST},   {"continue", TOK_KW_CONTINUE},
    {"else", TOK_KW_ELSE},     {"enum", TOK_KW_ENUM},
    {"false", TOK_KW_FALSE},   {"fn", TOK_KW_FN},
    {"for", TOK_KW_FOR},       {"if", TOK_KW_IF},
    {"impl", TOK_KW_IMPL},     {"in", TOK_KW_IN},
    {"let", TOK_KW_LET},       {"match", TOK_KW_MATCH},
    {"module", TOK_KW_MODULE}, {"mut", TOK_KW_MUT},
    {"pub", TOK_KW_PUB},       {"ref", TOK_KW_REF},
    {"return", TOK_KW_RETURN}, {"self", TOK_KW_SELF},
    {"Self", TOK_KW_SELFTYPE}, {"static", TOK_KW_STATIC},
    {"struct", TOK_KW_STRUCT}, {"super", TOK_KW_SUPER},
    {"trait", TOK_KW_TRAIT},   {"true", TOK_KW_TRUE},
    {"type", TOK_KW_TYPE},     {"use", TOK_KW_USE},
    {"var", TOK_KW_VAR},       {"while", TOK_KW_WHILE},
};

typedef struct lookup_result {
    char* remaining_chars;
    token_kind_t kind;
} lookup_result_t;

lookup_result_t lookup_operator(char* chars);

// lookup_result_t lookup_keyword(char* chars);

#endif
