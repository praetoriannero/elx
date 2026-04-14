#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "allocator.h"
#include "array.h"
#include "lexer.h"
#include "logger.h"
#include "modprim.h"
#include "str.h"
#include "todo.h"
#include "token.h"
#include "token_kind.h"
#include "token_tables.h"
#include "token_utils.h"
#include "xalloc.h"

void lexer_error(Lexer* self, char* msg) {
  // handle unrecoverable errors
  panic("lexing error: %s at location %d:%d in file '%s'\n", msg, self->context.line + 1, self->context.col,
        self->file_name);
}

void lexer_init(Lexer* self, const char* data, const char* file_name) {
  xnotnull(self);

  self->data = data;
  self->file_name = file_name;
  self->context.loc = 0;
  self->context.col = 0;
  self->context.length = strlen(data);
  self->context.line = 0;
}

typedef struct {
  String* number_str;
  const char* valid_digits;
  const char* valid_suffixes;
} NumberContext;

static const char* whitespace = " \t\n\r";

static const char* suffix_chars = "uif2346";

static const char* binary_digits = "01";

static const char* octal_digits = "01234567";

static const char* hexadecimal_digits = "0123456789ABCDEF";

static const char* decimal_digits = "0123456789";

static void lexer_consume_digits(Allocator* alloc, Lexer* self, String* str, const char* valid_digits) {
  while (true) {
    char c = lexer_peek_first(self);
    if (strchr(valid_digits, c)) {
      string_push(str, lexer_consume(self));
      continue;
    } else if (c == '_') {
      if (isdigit(lexer_peek_last(self)) && isdigit(lexer_peek_second(self))) {
        string_push(str, lexer_consume(self));
        continue;
      }

      lexer_error(self, "invalid usage of '_' in number format");
    }

    break;
  }
}

static void lexer_skip_whitespace(Lexer* self) {
  char c = lexer_peek_first(self);
  while (strchr(whitespace, c)) {
    c = lexer_consume(self);
    if (c == -1) {
      break;
    }
    c = lexer_peek_first(self);
  }
}

static void lexer_skip_comment(Lexer* self) {
  char c = lexer_peek_first(self);
  while (c != '\n') {
    c = lexer_consume(self);
    if (c == -1) {
      break;
    }
    c = lexer_peek_first(self);
  }
}

Token lexer_advance(Allocator* allocator, Lexer* self) {
  Allocator local_allocator;
  allocator_init(&local_allocator);

  char c;

  Token token;
  token_init(allocator, &token);

  token.loc = self->context.loc;

  lexer_skip_whitespace(self);
  c = lexer_consume(self);

  if (c == -1) {
    token.kind = TOK_EOF;
    return token;
  }

  token.kind = single_char_token[(u8)c];

  // string literal
  if (token.kind == TOK_DQUOTE) {
    string_push(&token.str, c);
    while (lexer_peek_first(self) != '"') {
      if (lexer_peek_first(self) == -1) {
        token.kind = TOK_EOF;
        return token;
      }
      string_push(&token.str, lexer_consume(self));
    }
    string_push(&token.str, lexer_consume(self));
    token.kind = TOK_STRING;
    goto fn_next_exit;
  }

  // operator
  if (token.kind != TOK_INVALID) {
    string_push(&token.str, c);

    const OperatorNode* op_iter = op_table;
    usize sub_table_size = 0;
    usize table_size = array_len(op_table);
    OperatorNode op_node;

    char c_next = lexer_peek_first(self);
    TokenKind kind_next = single_char_token[(u8)c_next];

    // get first operator node
    for (usize i = 0; i < table_size; i++) {
      op_node = op_iter[i];
      if (op_node.text == c) {
        op_iter = op_node.children;
        sub_table_size = op_node.child_count;
        break;
      }
    }

    while (op_iter && (kind_next != TOK_INVALID) && (!strchr(whitespace, c_next))) {
      bool found = false;
      for (usize i = 0; i < sub_table_size; i++) {
        op_node = op_iter[i];
        if (op_node.text == c_next) {
          string_push(&token.str, lexer_consume(self));
          op_iter = op_node.children;
          sub_table_size = op_node.child_count;
          token.kind = op_node.kind;
          found = true;
          break;
        }
      }

      if (!found) {
        break;
      }

      c_next = lexer_peek_first(self);
      kind_next = single_char_token[(u8)c_next];
    }

    goto fn_next_exit;
  }

  // identifier
  if (is_valid_ident_start(c)) {
    while (is_ident_char(c)) {
      string_push(&token.str, c);
      if (!is_ident_char(lexer_peek_first(self))) {
        break;
      }
      c = lexer_consume(self);
    }

    usize length = array_len(keyword_kind_table);
    for (u8 i = 0; i < length; i++) {
      StringToken st = keyword_kind_table[i];
      if (strcmp(st.text, token.str.data) == 0) {
        token.kind = st.kind;
        goto fn_next_exit;
      }
    }
    token.kind = TOK_IDENT;
    goto fn_next_exit;
  }

  // integer/float
  if (isdigit(c)) {
    string_push(&token.str, c);
    token.kind = TOK_INTEGER;

    char c_first = lexer_peek_first(self);
    const char* base_digits = decimal_digits;
    BaseKind base_kind = BASE_DECIMAL;

    // could be 0bXXX, 0oXXX, or 0xXXX
    if (c == '0') {
      if (c_first == 'b') {
        base_digits = binary_digits;
        base_kind = BASE_BINARY;
        string_push(&token.str, lexer_consume(self));
      } else if (c_first == 'o') {
        base_digits = octal_digits;
        base_kind = BASE_OCTAL;
        string_push(&token.str, lexer_consume(self));
      } else if (c_first == 'x') {
        base_digits = hexadecimal_digits;
        base_kind = BASE_HEXADECIMAL;
        string_push(&token.str, lexer_consume(self));
      }

      token.number.base_digits = base_digits;
      token.number.base_kind = base_kind;
    }

    lexer_consume_digits(allocator, self, &token.str, base_digits);

    c_first = lexer_peek_first(self);
    char c_second = lexer_peek_second(self);

    if (c_first == '.') {
      if (c_second == '.') {
        goto fn_next_exit;
      }

      // consume the decimal
      string_push(&token.str, lexer_consume(self));

      if (base_kind != BASE_DECIMAL) {
        lexer_error(self, "invalid floating point base value");
      }

      lexer_consume_digits(allocator, self, &token.str, base_digits);
      token.kind = TOK_FLOAT;
    }

    c_first = lexer_peek_first(self);
    if (c_first == 'e' || c_first == 'E') {
      if (base_kind != BASE_DECIMAL) {
        lexer_error(self, "invalid floating point base value");
      }

      // consume c_first
      string_push(&token.str, lexer_consume(self));

      if (c_second == '-' || c_second == '+') {
        // consume c_second
        string_push(&token.str, lexer_consume(self));
      }

      // consume the exponent
      lexer_consume_digits(allocator, self, &token.str, base_digits);
      token.kind = TOK_FLOAT;
    }

    // might still contain a suffix
    c_first = lexer_peek_first(self);
    while (strchr(suffix_chars, c_first)) {
      string_push(&token.str, lexer_consume(self));
      c_first = lexer_peek_first(self);
    }
  }

fn_next_exit:
  if (token.kind == TOK_COMMENT) {
    lexer_skip_comment(self);
    return lexer_next(allocator, self);
  }

  token.size = self->context.loc - token.loc;

  if (token.kind == TOK_INVALID) {
    lexer_error(self, "invalid token");
  }

  allocator_deinit(&local_allocator);
  return token;
}

Token lexer_next(Allocator* allocator, Lexer* self) {
  Token token = lexer_advance(allocator, self);
  log("%s\n", token_string(allocator, &token));
  return token;
}

Token lexer_peek(Allocator* allocator, Lexer* self) {
  LexerContext context = self->context;
  Token token = lexer_advance(allocator, self);
  self->context = context;
  return token;
}

i64 lexer_scan(Lexer* self, TokenKind key) {
  LexerContext start_context = self->context;
  Allocator local_allocator = {};
  i64 pos = -1;

  allocator_init(&local_allocator);

  while (true) {
    LexerContext cursor = self->context;
    Token token = lexer_next(&local_allocator, self);
    if (token.kind == key) {
      pos = (i64)cursor.loc;
      break;
    } else if (token.kind == TOK_EOF) {
      break;
    }
  }

  allocator_deinit(&local_allocator);
  self->context = start_context;

  return pos;
}

char lexer_peek_last(Lexer* self) {
  if (self->context.loc == 0) {
    panic("unexpected peek from beginning of file");
  }

  return self->data[self->context.loc - 1];
}

char lexer_peek_first(Lexer* self) { return self->data[self->context.loc]; }

char lexer_peek_second(Lexer* self) {
  if (self->context.loc == self->context.length - 1) {
    panic("unexpected end of file");
  }

  return self->data[self->context.loc + 1];
}

char lexer_consume(Lexer* self) {
  if (self->context.loc == self->context.length - 1) {
    return -1;
  }

  char c = self->data[self->context.loc++];

  if (c == '\n') {
    self->context.line++;
    self->context.col = 0;
  } else {
    self->context.col++;
  }

  return c;
}

void lexer_consume_into(Lexer* self, String* string, usize count) {
  for (usize idx = 0; idx < count; idx++) {
    string_push(string, lexer_consume(self));
  }
}
