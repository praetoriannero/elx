#pragma once

#include "array.h"
#include "modprim.h"

typedef enum {
  BASE_INVALID,
  BASE_DECIMAL,
  BASE_OCTAL,
  BASE_BINARY,
  BASE_HEXADECIMAL,
} BaseKind;

typedef enum {
  OP_QMARK,      // ?
  OP_DQUOTE,     // "
  OP_SQUOTE,     // '
  OP_EXCLAM,     // !
  OP_COLON,      // :
  OP_XOR,        // ^
  OP_PERCENT,    // %
  OP_BINV,       // ~
  OP_MINUS,      // -
  OP_PLUS,       // +
  OP_DOLLAR,     // $
  OP_TICK,       // `
  OP_STAR,       // *
  OP_FSLASH,     // /
  OP_BSLASH,     // \ .
  OP_LBRACE,     // {
  OP_RBRACE,     // }
  OP_AT,         // @
  OP_EQ,         // =
  OP_LPAREN,     // (
  OP_RPAREN,     // )
  OP_POUND,      // #
  OP_AMPER,      // &
  OP_LBRACK,     // [
  OP_RBRACK,     // ]
  OP_PIPE,       // |
  OP_GT,         // >
  OP_LT,         // <
  OP_SEMICOLON,  // ;
  OP_COMMA,      // ,
  OP_DECIMAL,    // .
  OP_UNIT,       // ()
  OP_LOR,        // ||
  OP_LAND,       // &&
  OP_SHL,        // <<
  OP_SHR,        // >>
  OP_GTE,        // >=
  OP_LTE,        // <=
  OP_ARROW,      // ->
  OP_COMMENT,    // //
  OP_RANGE,      // ..
  OP_RANGEINCL,  // ..=
  OP_PATH,       // ::
  OP_EQEQ,       // ==
  OP_NOTEQ,      // !=
  OP_PLUSEQ,     // +=
  OP_MINEQ,      // -=
  OP_TIMESEQ,    // *=
  OP_DIVEQ,      // /=
  OP_BINVEQ,     // ~=
  OP_OREQ,       // |=
  OP_ANDEQ,      // &=
  OP_XOREQ,      // ^=
  OP_MODEQ,      // %=
  OP_SHLEQ,      // <<=
  OP_SHREQ,      // >>=
  OP_DESTRUCTOR, // ~()
} OperatorKind;

typedef enum {
  SEP_LPAREN,
  SEP_RPAREN,
  SEP_LBRACK,
  SEP_RBRACK,
  SEP_LBRACE,
  SEP_RBRACE,
  SEP_COLON,
  SEP_SEMICOLON,
} SeparatorKind;

typedef enum {
  KW_AS,       // as
  KW_ASYNC,    // async
  KW_AWAIT,    // await
  KW_BREAK,    // break
  KW_CONST,    // const
  KW_CONTINUE, // continue
  KW_ELSE,     // else
  KW_ENUM,     // enum
  KW_FALSE,    // false
  KW_FN,       // fn
  KW_FOR,      // for
  KW_IF,       // if
  KW_IMPL,     // impl
  KW_IN,       // in
  KW_LET,      // let
  KW_VAR,      // var
  KW_MATCH,    // match
  KW_MODULE,   // module
  KW_PUB,      // pub
  KW_REF,      // ref
  KW_RETURN,   // return
  KW_SELF,     // self
  KW_SELFTYPE, // selftype
  KW_STATIC,   // static
  KW_STRUCT,   // struct
  KW_SUPER,    // super
  KW_TRAIT,    // trait
  KW_TRUE,     // true
  KW_TYPE,     // type
  KW_USE,      // use
  KW_WHILE,    // while
  KW_YIELD,    // yield
} KeywordKind;

typedef enum token_kind {
  // invalid/unsupported
  TOK_INVALID,

  // operators
  TOK_QMARK,     // ?
  TOK_DQUOTE,    // "
  TOK_SQUOTE,    // '
  TOK_EXCLAM,    // !
  TOK_COLON,     // :
  TOK_XOR,       // ^
  TOK_PERCENT,   // %
  TOK_BINV,      // ~
  TOK_MINUS,     // -
  TOK_PLUS,      // +
  TOK_DOLLAR,    // $
  TOK_TICK,      // `
  TOK_STAR,      // *
  TOK_FSLASH,    // /
  TOK_BSLASH,    // \ .
  TOK_LBRACE,    // {
  TOK_RBRACE,    // }
  TOK_AT,        // @
  TOK_EQ,        // =
  TOK_LPAREN,    // (
  TOK_RPAREN,    // )
  TOK_POUND,     // #
  TOK_AMPER,     // &
  TOK_LBRACK,    // [
  TOK_RBRACK,    // ]
  TOK_PIPE,      // |
  TOK_GT,        // >
  TOK_LT,        // <
  TOK_SEMICOLON, // ;
  TOK_COMMA,     // ,
  TOK_DECIMAL,   // .
  TOK_UNIT,      // ()

  // multi-char operators
  TOK_LOR,       // ||
  TOK_LAND,      // &&
  TOK_SHL,       // <<
  TOK_SHR,       // >>
  TOK_GTE,       // >=
  TOK_LTE,       // <=
  TOK_ARROW,     // ->
  TOK_COMMENT,   // //
  TOK_RANGE,     // ..
  TOK_RANGEINCL, // ..=
  TOK_PATH,      // ::

  // joint operator assignment
  TOK_EQEQ,       // ==
  TOK_NOTEQ,      // !=
  TOK_PLUSEQ,     // +=
  TOK_MINEQ,      // -=
  TOK_TIMESEQ,    // *=
  TOK_DIVEQ,      // /=
  TOK_BINVEQ,     // ~=
  TOK_OREQ,       // |=
  TOK_ANDEQ,      // &=
  TOK_XOREQ,      // ^=
  TOK_MODEQ,      // %=
  TOK_SHLEQ,      // <<=
  TOK_SHREQ,      // >>=
  TOK_DESTRUCTOR, // ~()

  // keywords
  TOK_KW_AS,       // as
  TOK_KW_ASYNC,    // async
  TOK_KW_AWAIT,    // await
  TOK_KW_BREAK,    // break
  TOK_KW_CONST,    // const
  TOK_KW_CONTINUE, // continue
  TOK_KW_ELSE,     // else
  TOK_KW_ENUM,     // enum
  TOK_KW_FALSE,    // false
  TOK_KW_FN,       // fn
  TOK_KW_FOR,      // for
  TOK_KW_IF,       // if
  TOK_KW_IMPL,     // impl
  TOK_KW_IN,       // in
  TOK_KW_LET,      // let
  TOK_KW_VAR,      // var
  TOK_KW_MATCH,    // match
  TOK_KW_MODULE,   // module
  // TOK_KW_MUT,      // mut
  TOK_KW_PUB,      // pub
  TOK_KW_REF,      // ref
  TOK_KW_RETURN,   // return
  TOK_KW_SELF,     // self
  TOK_KW_SELFTYPE, // selftype
  TOK_KW_STATIC,   // static
  TOK_KW_STRUCT,   // struct
  TOK_KW_SUPER,    // super
  TOK_KW_TRAIT,    // trait
  TOK_KW_TRUE,     // true
  TOK_KW_TYPE,     // type
  TOK_KW_USE,      // use
  TOK_KW_WHILE,    // while
  TOK_KW_YIELD,    // yield

  // identifier
  TOK_IDENT,

  // float
  TOK_FLOAT,

  // integer
  TOK_INTEGER,

  // string literal
  TOK_STRING,

  // end of file
  TOK_EOF,

  // empty string
  TOK_EMPTY
} TokenKind;

/*
 * - identifier
 * - keyword
 * - separator
 * - operator
 * - literal
 *   - integer
 *   - float
 *   - string
 * - comment
 * - whitespace
 */

static const TokenKind single_char_token[256] = {
    ['.'] = TOK_DECIMAL,   [','] = TOK_COMMA,   ['?'] = TOK_QMARK,  ['"'] = TOK_DQUOTE,  ['\''] = TOK_SQUOTE,
    ['!'] = TOK_EXCLAM,    [':'] = TOK_COLON,   ['^'] = TOK_XOR,    ['%'] = TOK_PERCENT, ['~'] = TOK_BINV,
    ['-'] = TOK_MINUS,     ['+'] = TOK_PLUS,    ['$'] = TOK_DOLLAR, ['`'] = TOK_TICK,    ['*'] = TOK_STAR,
    ['/'] = TOK_FSLASH,    ['\\'] = TOK_BSLASH, ['{'] = TOK_LBRACE, ['}'] = TOK_RBRACE,  ['@'] = TOK_AT,
    ['='] = TOK_EQ,        ['('] = TOK_LPAREN,  [')'] = TOK_RPAREN, ['#'] = TOK_POUND,   ['&'] = TOK_AMPER,
    ['['] = TOK_LBRACK,    [']'] = TOK_RBRACK,  ['|'] = TOK_PIPE,   ['>'] = TOK_GT,      ['<'] = TOK_LT,
    [';'] = TOK_SEMICOLON,
};

static const u32 valid_ops[] = {
    // operators
    TOK_QMARK, // ?
    // TOK_DQUOTE,    // "
    // TOK_SQUOTE,    // '
    // TOK_EXCLAM,    // !
    // TOK_COLON,     // :
    TOK_XOR,     // ^
    TOK_PERCENT, // %
    TOK_BINV,    // ~
    TOK_MINUS,   // -
    TOK_PLUS,    // +
    // TOK_DOLLAR,    // $
    // TOK_TICK,      // `
    TOK_STAR,   // *
    TOK_FSLASH, // /
    // TOK_BSLASH,    // \ .
    TOK_LBRACE, // {
    TOK_RBRACE, // }
    // TOK_AT,        // @
    TOK_EQ,     // =
    TOK_LPAREN, // (
    TOK_RPAREN, // )
    // TOK_POUND,     // #
    TOK_AMPER,  // &
    TOK_LBRACK, // [
    TOK_RBRACK, // ]
    TOK_PIPE,   // |
    TOK_GT,     // >
    TOK_LT,     // <
    // TOK_SEMICOLON, // ;
    // TOK_COMMA,     // ,
    TOK_DECIMAL, // .
    // TOK_UNIT,      // ()

    // multi-char operators
    TOK_LOR,        // ||
    TOK_LAND,       // &&
    TOK_SHL,        // <<
    TOK_SHR,        // >>
    TOK_GTE,        // >=
    TOK_LTE,        // <=
    TOK_ARROW,      // ->
    TOK_COMMENT,    // //
    TOK_RANGE,      // ..
    TOK_RANGEINCL,  // ..=
    TOK_PATH,       // ::
    TOK_DESTRUCTOR, // ~()

    // joint operator assignment
    TOK_EQEQ,    // ==
    TOK_NOTEQ,   // !=
    TOK_PLUSEQ,  // +=
    TOK_MINEQ,   // -=
    TOK_TIMESEQ, // *=
    TOK_DIVEQ,   // /=
    TOK_BINVEQ,  // ~=
    TOK_OREQ,    // |=
    TOK_ANDEQ,   // &=
    TOK_XOREQ,   // ^=
    TOK_MODEQ,   // %=
    TOK_SHLEQ,   // <<=
    TOK_SHREQ,   // >>=
};

static inline bool is_valid_op(TokenKind kind) {
  usize table_len = array_len(valid_ops);
  for (usize idx = 0; idx < table_len; idx++) {
    if (kind == valid_ops[idx]) {
      return true;
    }
  }

  return false;
}

char* token_kind_str(TokenKind kind);
