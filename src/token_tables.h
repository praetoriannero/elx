#ifndef ELX_TOKEN_TABLES_H
#define ELX_TOKEN_TABLES_H

#include <stdlib.h>

#include "token_kind.h"

struct op_node {
  char text;
  TokenKind kind;
  const struct op_node* children;
  usize child_count;
};

typedef struct op_node OperatorNode;

static const OperatorNode shr_children[] = {
    {'=', TOK_SHREQ, NULL, 0},
};

static const OperatorNode shl_children[] = {
    {'=', TOK_SHLEQ, NULL, 0},
};

static const OperatorNode mod_children[] = {
    {'=', TOK_MODEQ, NULL, 0},
};

static const OperatorNode xor_children[] = {
    {'=', TOK_XOREQ, NULL, 0},
};

static const OperatorNode binv_children[] = {
    {'=', TOK_BINVEQ, NULL, 0},
};

static const OperatorNode div_children[] = {
    {'=', TOK_DIVEQ, NULL, 0},
};

static const OperatorNode times_children[] = {
    {'=', TOK_TIMESEQ, NULL, 0},
};

static const OperatorNode min_children[] = {
    {'=', TOK_MINEQ, NULL, 0},
    {'>', TOK_ARROW, NULL, 0},
};

static const OperatorNode plus_children[] = {
    {'=', TOK_PLUSEQ, NULL, 0},
};

static const OperatorNode excl_children[] = {
    {'=', TOK_NOTEQ, NULL, 0},
};

static const OperatorNode eq_children[] = {
    {'=', TOK_EQEQ, NULL, 0},
};

static const OperatorNode gt_children[] = {
    {'>', TOK_SHR, shr_children, 1},
    {'=', TOK_GTE, NULL, 0},
};

static const OperatorNode lt_children[] = {
    {'<', TOK_SHL, shl_children, 1},
    {'=', TOK_LTE, NULL, 0},
};

static const OperatorNode and_children[] = {
    {'&', TOK_LAND, NULL, 0},
    {'=', TOK_ANDEQ, NULL, 0},
};

static const OperatorNode or_children[] = {
    {'|', TOK_LOR, NULL, 0},
    {'=', TOK_OREQ, NULL, 0},
};

static const OperatorNode fs_children[] = {
    {'/', TOK_COMMENT, NULL, 0},
    {'=', TOK_DIVEQ, NULL, 0},
};

static const OperatorNode range_children[] = {{'=', TOK_RANGEINCL, NULL, 0}};

static const OperatorNode dec_children[] = {
    {'.', TOK_RANGE, range_children, 1},
};

static const OperatorNode colon_children[] = {
    {':', TOK_PATH, NULL, 0},
};

static const OperatorNode star_children[] = {
    {'=', TOK_TIMESEQ, NULL, 0},
};

static const OperatorNode op_table[] = {
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

typedef struct {
  char* text;
  TokenKind kind;
} StringToken;

static const StringToken keyword_kind_table[] = {
    {"as", TOK_KW_AS},         {"async", TOK_KW_ASYNC},   {"await", TOK_KW_AWAIT},
    {"break", TOK_KW_BREAK},   {"const", TOK_KW_CONST},   {"continue", TOK_KW_CONTINUE},
    {"else", TOK_KW_ELSE},     {"enum", TOK_KW_ENUM},     {"false", TOK_KW_FALSE},
    {"fn", TOK_KW_FN},         {"for", TOK_KW_FOR},       {"if", TOK_KW_IF},
    {"impl", TOK_KW_IMPL},     {"in", TOK_KW_IN},         {"let", TOK_KW_LET},
    {"match", TOK_KW_MATCH},   {"module", TOK_KW_MODULE}, {"pub", TOK_KW_PUB},
    {"ref", TOK_KW_REF},       {"return", TOK_KW_RETURN}, {"self", TOK_KW_SELF},
    {"Self", TOK_KW_SELFTYPE}, {"static", TOK_KW_STATIC}, {"struct", TOK_KW_STRUCT},
    {"super", TOK_KW_SUPER},   {"trait", TOK_KW_TRAIT},   {"true", TOK_KW_TRUE},
    {"type", TOK_KW_TYPE},     {"use", TOK_KW_USE},       {"var", TOK_KW_VAR},
    {"while", TOK_KW_WHILE},   {"yield", TOK_KW_YIELD},
    // {"mut", TOK_KW_MUT},
};

typedef struct lookup_result {
  char* remaining_chars;
  TokenKind kind;
} LookupResult;

LookupResult lookup_operator(char* chars);

// lookup_result_t lookup_keyword(char* chars);

#endif
