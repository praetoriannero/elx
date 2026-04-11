#ifndef ELX_TOKEN_KIND_H
#define ELX_TOKEN_KIND_H

#include <stdlib.h>

#include "array.h"
#include "modprim.h"

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
  TOK_EMTPY
} TokenKind;

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

#endif
