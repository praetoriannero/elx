#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "core/allocator.h"
#include "core/logger.h" // IWYU pragma: keep
#include "core/modprim.h"
#include "core/panic.h"
#include "core/todo.h"
#include "core/vector.h"
#include "core/xalloc.h"
#include "lexer/lexer.h"
#include "lexer/span.h"
#include "parser/parser.h"
#include "token/token.h"
#include "token/token_kind.h"

static char* UNSUPPORTED_IDENTIFIER = "UNSUPPORTED";

ExprPrecedence postfix_precedence(TokenKind kind) {
  switch (kind) {
    case TOK_LBRACE:
    case TOK_LPAREN:
    case TOK_DECIMAL:
    case TOK_LBRACK:
      return EXPR_PREC_POSTFIX;
    default:
      return 0;
  }
}

ExprPrecedence infix_precedence(TokenKind kind) {
  switch (kind) {
    case TOK_LOR:
      return EXPR_PREC_LOR;

    case TOK_LAND:
      return EXPR_PREC_LAND;

    case TOK_EQEQ:
    case TOK_NOTEQ:
    case TOK_LT:
    case TOK_LTE:
    case TOK_GT:
    case TOK_GTE:
      return EXPR_PREC_COMP;

    case TOK_PIPE:
      return EXPR_PREC_LOR;

    case TOK_XOR:
      return EXPR_PREC_BXOR;

    case TOK_AMPER:
      return EXPR_PREC_BAND;

    case TOK_SHL:
    case TOK_SHR:
      return EXPR_PREC_SHIFT;

    case TOK_PLUS:
    case TOK_MINUS:
      return EXPR_PREC_ADDSUB;

    case TOK_STAR:
    case TOK_FSLASH:
      return EXPR_PREC_MULDIV;

    case TOK_EQ:
    case TOK_PLUSEQ:
    case TOK_MINEQ:
    case TOK_TIMESEQ:
    case TOK_DIVEQ:
    case TOK_MODEQ:
    case TOK_XOREQ:
    case TOK_ANDEQ:
    case TOK_OREQ:
    case TOK_SHLEQ:
    case TOK_SHREQ:
      return EXPR_PREC_ASSIGN;

    case TOK_RANGE:
    case TOK_RANGEINCL:
      return EXPR_PREC_RANGE;

    case TOK_PATH:
      return EXPR_PREC_UNAMBIGUOUS;

    default:
      return 0;
  }
}

static void parser_unexpected_token_error(Span* span, Allocator* alloc) {
  SpanContext span_ctx = span_get_context(span, alloc);
  panic("\n\tin %s at %d:%d\n\tunexpected token '%s'\t\n\t%s\n\t%s\n", span->path, span_ctx.line, span_ctx.column,
        span_ctx.token_context, span_ctx.line_str.data, span_ctx.underline_str.data);
}

Token parser_expect(TokenKind expected, Token actual) {
  scoped_allocator(panic_alloc);
  if (expected != actual.kind) {
    SpanContext span_ctx = span_get_context(&actual.span, &panic_alloc);
    panic("in %s %d:%d\n\texpected token kind %s but found %s\n\t%s\n%s\n", actual.span.path, span_ctx.line,
          span_ctx.column, token_kind_str(expected), span_ctx.token_context, span_ctx.line_str.data,
          span_ctx.underline_str.data);
  }

  return actual;
}

Expr* parse_expr_prec(Parser* self, TokenKind stop_token, ExprPrecedence min_prec) {
  debug("stop token %s\n", token_kind_str(stop_token));

  Expr* lhs = allocator_alloc(self->alloc, sizeof(Expr));
  Expr* prev_lhs;
  Token left_tok = lexer_peek(self->lexer);

  if (left_tok.kind == stop_token) {
    // empty expression
    debug("lhs end of expr\n");
    lhs->expr_kind = EXPR_KIND_EMPTY;
    return lhs;
  }

  left_tok = lexer_next(self->lexer);

  if (left_tok.kind == TOK_IDENT) {
    // identifier
    debug("identifier expr\n");
    lhs->expr_kind = EXPR_KIND_IDENT;
    lhs->ident_expr = left_tok.span;

    // todo: missing byte and char literal support in AST
  } else if (left_tok.kind == TOK_KW_SELF) {
    lhs->expr_kind = EXPR_KIND_SELF;
    lhs->self_expr = left_tok.span;
  } else if (left_tok.kind == TOK_FLOAT) {
    // float literal
    debug("float expr\n");
    lhs->expr_kind = EXPR_KIND_LITERAL;
    lhs->literal_expr.kind = LITERAL_KIND_FLOAT;
    lhs->literal_expr.float_ = left_tok.span;

  } else if (left_tok.kind == TOK_INTEGER) {
    // integer literal
    debug("integer expr\n");
    lhs->expr_kind = EXPR_KIND_LITERAL;
    lhs->literal_expr.kind = LITERAL_KIND_INTEGER;
    lhs->literal_expr.integer = left_tok.span;

  } else if (left_tok.kind == TOK_STRING) {
    // string literal
    debug("string expr\n");
    lhs->expr_kind = EXPR_KIND_LITERAL;
    lhs->literal_expr.kind = LITERAL_KIND_STRING;
    lhs->literal_expr.string = left_tok.span;

  } else if ((left_tok.kind == TOK_KW_FALSE) || (left_tok.kind == TOK_KW_TRUE)) {
    debug("bool expr\n");
    lhs->expr_kind = EXPR_KIND_LITERAL;
    lhs->literal_expr.kind = LITERAL_KIND_BOOL;
    lhs->literal_expr.bool_ = left_tok.span;

  } else if (left_tok.kind == TOK_LBRACK) {
    // array implicit literal, ex: [u32; 10]
    u64 comma_count = count_csv(self->lexer->data + self->lexer->context.loc, '(');
    if (!comma_count) {
      debug("array implicit expr\n");
      lhs->expr_kind = EXPR_KIND_ARRAY_IMPLICIT;

      Expr* element_type = parse_expr_prec(self, TOK_SEMICOLON, 0);
      parser_expect(TOK_SEMICOLON, lexer_next(self->lexer));
      Expr* element_count = parse_expr_prec(self, TOK_RBRACK, 0);
      parser_expect(TOK_RBRACK, lexer_next(self->lexer));

      lhs->array_implicit_expr.kind = element_type;
      lhs->array_implicit_expr.size = element_count;

    } else {
      // array explicit literal, ex: [1, 2, 3]
      debug("array explicit expr\n");
      lhs->expr_kind = EXPR_KIND_ARRAY_EXPLICIT;
      for (u64 i = 0; i < comma_count; i++) {
        Expr* arg_expr = parse_expr_prec(self, TOK_COMMA, 0);
        parser_expect(TOK_COMMA, lexer_next(self->lexer));
        vector_push(&lhs->array_explicit_expr.arg_vec, arg_expr);
      }

      if (lexer_peek(self->lexer).kind != TOK_RBRACK) {
        Expr* arg_expr = parse_expr_prec(self, TOK_RBRACK, 0);
        parser_expect(TOK_RBRACK, lexer_next(self->lexer));
        vector_push(&lhs->array_explicit_expr.arg_vec, arg_expr);
      }
    }

  } else if (left_tok.kind == TOK_LPAREN) {
    // group expression
    // todo: support tuples, ex: (1, 2, 3)
    debug("group expr\n");
    lhs = parse_expr_prec(self, TOK_RPAREN, 0);
    parser_expect(TOK_RPAREN, lexer_next(self->lexer));

  } else if (is_valid_unary_op(left_tok.kind)) {
    // unary expression
    debug("unary expr\n");
    lhs->expr_kind = EXPR_KIND_UNARY;
    lhs->unary_expr.op = *token_copy(self->alloc, &left_tok);
    lhs->unary_expr.inner = parse_expr_prec(self, stop_token, EXPR_PREC_PREFIX - 1);
  } else {
    // error
    parser_unexpected_token_error(&left_tok.span, self->alloc);
  }

  while (true) {
    debug("checking rhs\n");
    Token operator_tok = lexer_peek(self->lexer);
    if (operator_tok.kind == stop_token) {
      debug("rhs end of expr\n");
      // end of expression
      break;
    }

    if (!is_valid_op(operator_tok.kind)) {
      // invalid operator
      panic("invalid token encountered during expression parse: '%s'", token_string(self->alloc, &operator_tok));
    }

    ExprPrecedence postfix_prec = postfix_precedence(operator_tok.kind);
    if (postfix_prec) {
      if (postfix_prec < min_prec) {
        debug("postfix break\n");
        break;
      }

      operator_tok = lexer_next(self->lexer);

      if (operator_tok.kind == TOK_LBRACE) {
        // struct init expression
        debug("struct init expr\n");
        prev_lhs = lhs;

        lhs = allocator_alloc(self->alloc, sizeof(Expr));
        lhs->expr_kind = EXPR_KIND_STRUCT;

        lhs->struct_init_expr.object = prev_lhs;
        vector_init(&lhs->struct_init_expr.arg_vec, self->alloc, sizeof(Expr), 1, NULL);

        u64 comma_count = count_csv(self->lexer->data + self->lexer->context.loc, '{');
        for (u64 i = 0; i < comma_count; i++) {
          Expr* arg_expr = parse_expr_prec(self, TOK_COMMA, 0);
          parser_expect(TOK_COMMA, lexer_next(self->lexer));
          vector_push(&lhs->struct_init_expr.arg_vec, arg_expr);
        }

        if (lexer_peek(self->lexer).kind != TOK_RBRACE) {
          Expr* arg_expr = parse_expr_prec(self, TOK_RBRACE, 0);
          vector_push(&lhs->struct_init_expr.arg_vec, arg_expr);
        }

        parser_expect(TOK_RBRACE, lexer_next(self->lexer));
        continue;

      } else if (operator_tok.kind == TOK_LPAREN) {
        // call or group expression
        if (lhs->expr_kind == EXPR_KIND_BINARY) {
          // group expression
          panic("unhandled binary expression preceding group - edge "
                "case\n");
        } else {
          // call expression
          debug("call expr\n");
          prev_lhs = lhs;

          lhs = allocator_alloc(self->alloc, sizeof(Expr));
          lhs->expr_kind = EXPR_KIND_CALL;
          lhs->call_expr.object = prev_lhs;
          vector_init(&lhs->call_expr.arg_vec, self->alloc, sizeof(Expr), 1, NULL);

          u64 comma_count = count_csv(self->lexer->data + self->lexer->context.loc, '(');
          for (u64 i = 0; i < comma_count; i++) {
            Expr* arg_expr = parse_expr_prec(self, TOK_COMMA, 0);
            parser_expect(TOK_COMMA, lexer_next(self->lexer));
            vector_push(&lhs->call_expr.arg_vec, arg_expr);
          }

          if (lexer_peek(self->lexer).kind != TOK_RPAREN) {
            Expr* arg_expr = parse_expr_prec(self, TOK_RPAREN, 0);
            vector_push(&lhs->call_expr.arg_vec, arg_expr);
          }

          parser_expect(TOK_RPAREN, lexer_next(self->lexer));
          continue;
        }

      } else if (operator_tok.kind == TOK_DECIMAL) {
        // field-type expression
        operator_tok = lexer_next(self->lexer); // field
        if (TOK_IDENT != operator_tok.kind) {
          panic("expected field identifier after token '.' but found "
                "'%.*s'",
                span_cstr(operator_tok.span));
        }

        Token lparen = lexer_peek(self->lexer); // possible method call
        if (lparen.kind == TOK_LPAREN) {
          lparen = lexer_next(self->lexer); // method call
          // method call expression
          debug("method call expr\n");
          prev_lhs = lhs;

          lhs = allocator_alloc(self->alloc, sizeof(Expr));
          vector_init(&lhs->method_call_expr.arg_vec, self->alloc, sizeof(Expr), 1, NULL);

          lhs->expr_kind = EXPR_KIND_METHOD_CALL;
          lhs->method_call_expr.object = prev_lhs;
          lhs->method_call_expr.method = operator_tok.span;

          u64 comma_count = count_csv(self->lexer->data + self->lexer->context.loc, '(');
          for (u64 i = 0; i < comma_count; i++) {
            Expr* arg_expr = parse_expr_prec(self, TOK_COMMA, 0);
            parser_expect(TOK_COMMA, lexer_next(self->lexer));
            vector_push(&lhs->method_call_expr.arg_vec, arg_expr);
          }

          if (lexer_peek(self->lexer).kind != TOK_RPAREN) {
            Expr* arg_expr = parse_expr_prec(self, TOK_RPAREN, 0);
            parser_expect(TOK_RPAREN, lexer_next(self->lexer));
            vector_push(&lhs->method_call_expr.arg_vec, arg_expr);
          }

          continue;
        }

        prev_lhs = lhs;

        lhs = allocator_alloc(self->alloc, sizeof(Expr));
        lhs->expr_kind = EXPR_KIND_FIELD;
        lhs->field_expr.object = prev_lhs;
        lhs->field_expr.span = operator_tok.span;
        continue;

      } else if (operator_tok.kind == TOK_LBRACK) {
        // array index expression
        debug("array index expr\n");
        prev_lhs = lhs;
        lhs = allocator_alloc(self->alloc, sizeof(Expr));
        lhs->expr_kind = EXPR_KIND_ARRAY_INDEX;
        lhs->array_index_expr.object = prev_lhs;
        lhs->array_index_expr.index = parse_expr_prec(self, TOK_RBRACK, EXPR_PREC_POSTFIX - 1);
        parser_expect(TOK_RBRACK, lexer_next(self->lexer));
        continue;
      }
    }

    ExprPrecedence infix_prec = infix_precedence(operator_tok.kind);
    if (infix_prec) {
      if (infix_prec < min_prec) {
        break;
      }

      operator_tok = lexer_next(self->lexer);

      if (operator_tok.kind == TOK_PATH) {
        debug("path expr\n");

        prev_lhs = lhs;
        lhs = allocator_alloc(self->alloc, sizeof(Expr));
        lhs->expr_kind = EXPR_KIND_PATH;
        assert(prev_lhs->expr_kind == EXPR_KIND_IDENT);
        lhs->path_expr.stem = prev_lhs->ident_expr;
        lhs->path_expr.expr = parse_expr_prec(self, stop_token, infix_prec - 1);
        continue;

      } else {
        debug("binary expr %s\n", token_kind_str(operator_tok.kind));

        prev_lhs = lhs;
        lhs = allocator_alloc(self->alloc, sizeof(Expr));
        lhs->expr_kind = EXPR_KIND_BINARY;
        lhs->binary_expr.lhs = prev_lhs;
        lhs->binary_expr.op = *token_copy(self->alloc, &operator_tok);
        lhs->binary_expr.rhs = parse_expr_prec(self, stop_token, infix_prec + 1);
        continue;
      }
    }

    // if (lexer_peek(&local_arena, self->lexer).kind != stop_token) {
    //     continue;
    // }

    debug("end of expr\n");
    debug("next token %s\n", token_kind_str(operator_tok.kind));
    break;
  }

  return lhs;
}

Expr* parser_visit_expr(Parser* self, TokenKind stop_token) {
  /*
   * Consume tokens until we reach the stop token, creating an expression tree
   */
  return parse_expr_prec(self, stop_token, 0);
}

Stmt parser_visit_break_stmt(Parser* self) {
  // scope_t* scope = arena_new_scope(arena);

  parser_expect(TOK_KW_BREAK, lexer_next(self->lexer));
  Stmt stmt = {};

  stmt.kind = STMT_KIND_BREAK;

  // todo: support continue by explicitly listing loop identifier
  stmt.break_stmt.ident =
      (Span){.buffer = UNSUPPORTED_IDENTIFIER, .hi = strlen(UNSUPPORTED_IDENTIFIER), .lo = 0, .path = ""};

  // arena_free_scope(arena, scope);
  return stmt;
}

Stmt parser_visit_expr_stmt(Parser* self) {
  Stmt stmt = {};

  Expr* expr = parser_visit_expr(self, TOK_SEMICOLON);

  // scope_t* scope = arena_new_scope(arena);
  parser_expect(TOK_SEMICOLON, lexer_next(self->lexer));
  // arena_free_scope(arena, scope);

  stmt.kind = STMT_KIND_EXPR;
  stmt.expr_stmt.expr = expr;
  return stmt;
}

Stmt parser_visit_for_stmt(Parser* self) {
  Stmt stmt = {};
  Token feed = {};

  parser_expect(TOK_KW_FOR, lexer_next(self->lexer));

  feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  stmt.kind = STMT_KIND_FOR;

  stmt.for_stmt.iterator = feed.span;
  parser_expect(TOK_KW_IN, lexer_next(self->lexer));
  stmt.for_stmt.iterable = parser_visit_expr(self, TOK_LBRACE);
  parser_expect(TOK_LBRACE, lexer_next(self->lexer));
  stmt.for_stmt.body = parser_visit_body(self);
  parser_expect(TOK_RBRACE, lexer_next(self->lexer));

  return stmt;
}

Stmt parser_visit_if_stmt(Parser* self) {
  Stmt stmt = {};
  Token feed = {};

  // 'if' token
  feed = parser_expect(TOK_KW_IF, lexer_next(self->lexer));

  stmt.kind = STMT_KIND_IF;
  stmt.if_stmt.condition = parser_visit_expr(self, TOK_LBRACE);

  parser_expect(TOK_LBRACE, lexer_next(self->lexer));

  stmt.if_stmt.body = parser_visit_body(self);
  vector_init(&stmt.if_stmt.elif_clause_vec, self->alloc, sizeof(ElifClause), 1, NULL);

  parser_expect(TOK_RBRACE, lexer_next(self->lexer));

  while (true) {
    feed = lexer_peek(self->lexer);
    if (feed.kind != TOK_KW_ELSE) {
      break;
    }

    parser_expect(TOK_KW_ELSE, lexer_next(self->lexer));

    feed = lexer_peek(self->lexer); // maybe has 'if'
    if (feed.kind == TOK_KW_IF) {
      ElifClause elif_clause = {};
      parser_expect(TOK_KW_IF, lexer_next(self->lexer));
      elif_clause.condition = parser_visit_expr(self, TOK_LBRACE);
      parser_expect(TOK_LBRACE, lexer_next(self->lexer));
      elif_clause.body = parser_visit_body(self);
      parser_expect(TOK_RBRACE, lexer_next(self->lexer));
      vector_push(&stmt.if_stmt.elif_clause_vec, &elif_clause);

    } else {
      ElseClause else_clause = {};
      parser_expect(TOK_LBRACE, lexer_next(self->lexer));
      else_clause.body = parser_visit_body(self);
      parser_expect(TOK_RBRACE, lexer_next(self->lexer));
      stmt.if_stmt.else_clause = else_clause;
      break;
    }
  }

  return stmt;
}

Stmt parser_visit_while_stmt(Parser* self) {
  Stmt stmt = {};

  parser_expect(TOK_KW_WHILE, lexer_next(self->lexer));
  stmt.kind = STMT_KIND_WHILE;
  stmt.while_stmt.condition = parser_visit_expr(self, TOK_LBRACE);
  parser_expect(TOK_LBRACE, lexer_next(self->lexer));
  stmt.while_stmt.body = parser_visit_body(self);
  parser_expect(TOK_RBRACE, lexer_next(self->lexer));

  return stmt;
}

Stmt parser_visit_return_stmt(Parser* self) {
  debug("entering return stmt\n");
  Stmt stmt = {};

  parser_expect(TOK_KW_RETURN, lexer_next(self->lexer));

  stmt.kind = STMT_KIND_RETURN;
  stmt.return_stmt.expr = parser_visit_expr(self, TOK_SEMICOLON);

  parser_expect(TOK_SEMICOLON, lexer_next(self->lexer));

  debug("exiting return stmt\n");
  return stmt;
}

Stmt parser_visit_continue_stmt(Parser* self) {
  parser_expect(TOK_KW_CONTINUE, lexer_next(self->lexer));
  Stmt stmt = {};
  stmt.kind = STMT_KIND_CONTINUE;
  // todo: support continue by explicitly listing loop identifier
  stmt.cont_stmt.ident =
      (Span){.buffer = UNSUPPORTED_IDENTIFIER, .hi = strlen(UNSUPPORTED_IDENTIFIER), .lo = 0, .path = ""};

  return stmt;
}

Stmt parser_visit_assign_stmt(Parser* self) {
  Stmt stmt = {};

  stmt.kind = STMT_KIND_ASSIGN;

  bool mut = false;
  Token feed = lexer_next(self->lexer);
  if (feed.kind == TOK_KW_VAR) {
    mut = true;
  } else {
    parser_expect(TOK_KW_LET, feed);
  }
  stmt.assign_stmt.mut = mut;

  feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  stmt.assign_stmt.span = feed.span;

  parser_expect(TOK_EQ, lexer_next(self->lexer));
  stmt.assign_stmt.expr = parser_visit_expr(self, TOK_SEMICOLON);
  parser_expect(TOK_SEMICOLON, lexer_next(self->lexer));

  return stmt;
}

Body parser_visit_body(Parser* self) {
  debug("entering parser_visit_body\n");
  Body body = {};
  vector_init(&body.stmts, self->alloc, sizeof(Stmt), 4, NULL);

  Stmt stmt = {};

  Allocator scratch = {};
  allocator_init(&scratch);

  char* tok_str = NULL;

  while (true) {
    Token feed = lexer_peek(self->lexer);
    switch (feed.kind) {
      case TOK_KW_TRUE:
      case TOK_KW_FALSE:
      case TOK_INTEGER:
      case TOK_FLOAT:
      case TOK_IDENT:
        stmt = parser_visit_expr_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_FOR:
        stmt = parser_visit_for_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_IF:
        stmt = parser_visit_if_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_WHILE:
        stmt = parser_visit_while_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_RETURN:
        stmt = parser_visit_return_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_CONTINUE:
        stmt = parser_visit_continue_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_BREAK:
        stmt = parser_visit_break_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_KW_VAR:
      case TOK_KW_LET:
        stmt = parser_visit_assign_stmt(self);
        vector_push(&body.stmts, &stmt);
        continue;

      case TOK_RBRACE:
        break;

      default:
        SpanContext span_ctx = span_get_context(&feed.span, &scratch);
        panic("\n\tin %s at %d:%d\n\tunexpected token '%s'\t\n\t%s\n\t%s\n", feed.span.path, span_ctx.line,
              span_ctx.column, span_ctx.token_context, span_ctx.line_str.data, span_ctx.underline_str.data);
    }
    break;
  }

  allocator_deinit(&scratch);
  debug("leaving parser_visit_body\n");
  return body;
}

Vector parser_visit_module_inner(Parser* self, bool is_source) {
  debug("entering parser_visit_module_inner\n");
  xnotnull(self);

  Token token = {};
  char* tok_str = NULL;
  Allocator local_allocator = {};
  AstNode ast_node = {};
  Vector ast_node_vec = {};

  allocator_init(&local_allocator);
  vector_init(&ast_node_vec, self->alloc, sizeof(AstNode), 8, NULL);

  while (true) {
    debug("module inner loop start\n");
    allocator_deinit(&local_allocator);
    token = lexer_peek(self->lexer);

    switch (token.kind) {
      case TOK_COMMENT:
        continue;

      case TOK_KW_STRUCT:
        ast_node = parser_visit_struct(self);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_KW_MODULE:
        ast_node = parser_visit_module(self);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_KW_VAR:
        ast_node = parser_visit_global(self, true);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_KW_LET:
        ast_node = parser_visit_global(self, false);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_KW_ENUM:
        ast_node = parser_visit_enum(self);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_KW_USE:
        ast_node = parser_visit_import(self);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_KW_FN:
        ast_node = parser_visit_func(self);
        vector_push(&ast_node_vec, &ast_node);
        continue;

      case TOK_EOF:
        break;

      case TOK_RBRACE:
        if (!is_source) {
          break;
        }

      default:
        SpanContext span_ctx = span_get_context(&token.span, &local_allocator);
        panic("\n\tin %s at %d:%d\n\tunexpected token '%s'\t\n\t%s\n\t%s\n", token.span.path, span_ctx.line,
              span_ctx.column, span_ctx.token_context, span_ctx.line_str.data, span_ctx.underline_str.data);
    }

    break;
  }

  allocator_deinit(&local_allocator);
  debug("leaving parser_visit_module_inner\n");

  return ast_node_vec;
}

Ast parser_parse(Parser* self) {
  debug("entering parser_parse\n");
  Ast ast = {};
  Module module = {.span = {}, parser_visit_module_inner(self, true)};
  vector_init(&ast.module_vec, self->alloc, sizeof(Module), 1, NULL);
  vector_push(&ast.module_vec, &module);

  return ast;
}

AstNode parser_visit_struct(Parser* self) {
  debug("visiting struct\n");

  AstNode ast_node = {};
  Struct struct_ = {};

  vector_init(&struct_.field_vec, self->alloc, sizeof(StructField), 1, NULL);
  vector_init(&struct_.method_vec, self->alloc, sizeof(AstNode), 1, NULL);

  parser_expect(TOK_KW_STRUCT, lexer_next(self->lexer));

  // struct name
  Token feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  struct_.span = feed.span;
  ast_node.span = struct_.span;

  parser_expect(TOK_LBRACE, lexer_next(self->lexer));

  feed = lexer_peek(self->lexer);

  // struct contents
  while (feed.kind != TOK_RBRACE) {
    if (feed.kind == TOK_IDENT) { // struct field
      feed = lexer_next(self->lexer);
      StructField field = {};

      field.span = feed.span;

      parser_expect(TOK_COLON, lexer_next(self->lexer));

      feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
      field.type.span = feed.span;
      vector_push(&struct_.field_vec, &field);

      parser_expect(TOK_SEMICOLON, lexer_next(self->lexer));

    } else if (feed.kind == TOK_KW_FN) { // struct method
      AstNode method = parser_visit_func(self);
      vector_push(&struct_.method_vec, &method);

    } else { // unexpected input
      panic("unexpected token '%.*s'", span_cstr(feed.span));
    }

    // advance stream
    feed = lexer_peek(self->lexer);
  }

  parser_expect(TOK_RBRACE, lexer_next(self->lexer));

  ast_node.kind = AST_NODE_KIND_STRUCT;
  ast_node.struct_case = struct_;

  debug("leaving struct\n");
  return ast_node;
}

AstNode parser_visit_module(Parser* self) {
  debug("visiting module\n");

  AstNode ast_node = {};
  Module module = {};

  vector_init(&module.ast_node_vec, self->alloc, sizeof(AstNode), 8, NULL);

  parser_expect(TOK_KW_MODULE, lexer_next(self->lexer));

  // module name
  Token feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  module.span = feed.span;
  ast_node.span = module.span;

  // module content
  parser_expect(TOK_LBRACE, lexer_next(self->lexer));
  module.ast_node_vec = parser_visit_module_inner(self, false);
  parser_expect(TOK_RBRACE, lexer_next(self->lexer));

  ast_node.kind = AST_NODE_KIND_MODULE;
  ast_node.module_case = module;

  debug("leaving module\n");
  return ast_node;
}

AstNode parser_visit_func(Parser* self) {
  debug("visiting func\n");
  xnotnull(self->alloc);
  xnotnull(self);

  AstNode ast_node = {};
  Func func = {};

  parser_expect(TOK_KW_FN, lexer_next(self->lexer));
  vector_init(&func.param_vec, self->alloc, sizeof(FuncArg), 1, NULL);

  // function name
  Token feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  func.span = feed.span;
  ast_node.span = func.span;

  parser_expect(TOK_LPAREN, lexer_next(self->lexer));

  feed = lexer_next(self->lexer);

  // function arguments
  while (feed.kind != TOK_RPAREN) {
    FuncArg arg = {};
    arg.span = feed.span;
    parser_expect(TOK_COLON, lexer_next(self->lexer));
    feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
    arg.type.span = feed.span;
    vector_push(&func.param_vec, &arg);

    feed = lexer_next(self->lexer);
    if (feed.kind == TOK_COMMA) {
      feed = lexer_next(self->lexer);
    }
  }

  parser_expect(TOK_RPAREN, feed);

  // todo: allow empty return type
  parser_expect(TOK_ARROW, lexer_next(self->lexer));
  feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  func.ret_type.span = feed.span;

  parser_expect(TOK_LBRACE, lexer_next(self->lexer));

  func.body = parser_visit_body(self);

  parser_expect(TOK_RBRACE, lexer_next(self->lexer));

  ast_node.kind = AST_NODE_KIND_FUNC;
  ast_node.func_case = func;

  debug("leaving func\n");
  return ast_node;
}

AstNode parser_visit_global(Parser* self, bool is_var) {
  /*
   *  Parse a global identifier declaration
   *  examples:
   *      let x: u32 = 0;
   *      let mut foo: vec<std::String> = vec::new();
   *
   *  Initially, we expect an identifier or a `mut` keyword. Next, we
   *  must have a type specified for the global, and finally we need
   *  an assignment on the right hand side.
   *
   */

  debug("visiting global\n");

  Token feed = {};
  AstNode ast_node = {};
  Global global = {};

  feed = lexer_next(self->lexer);
  global.mut = is_var;

  // variable name
  feed = parser_expect(TOK_IDENT, feed);
  global.span = feed.span;
  ast_node.span = global.span;

  feed = parser_expect(TOK_COLON, lexer_next(self->lexer));

  // type name
  feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  global.type.span = feed.span;

  // globals must always be initialized
  feed = parser_expect(TOK_EQ, lexer_next(self->lexer));

  // remaining tokens become expression
  global.expr = parser_visit_expr(self, TOK_SEMICOLON);

  ast_node.kind = AST_NODE_KIND_GLOBAL;
  ast_node.global_case = global;

  debug("leaving global\n");
  return ast_node;
}

AstNode parser_visit_enum(Parser* self) {
  debug("visiting enum\n");
  xnotnull(self->alloc);
  xnotnull(self);

  AstNode ast_node = {};
  Enum enum_ = {};
  Token feed = {};

  feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
  enum_.span = feed.span;
  ast_node.span = enum_.span;

  parser_expect(TOK_LBRACE, lexer_next(self->lexer));

  feed = lexer_next(self->lexer);
  while (feed.kind != TOK_RBRACE) {
    if (feed.kind == TOK_EOF) {
      panic("invalid syntax\n");
    }

    EnumKind* enum_kind = allocator_alloc(self->alloc, sizeof(EnumKind));

    parser_expect(TOK_IDENT, feed);
    enum_kind->span = feed.span;

    feed = lexer_next(self->lexer);
    if (feed.kind == TOK_COMMA) {
      enum_kind->variant = ENUM_VARIANT_NOM;
      feed = lexer_next(self->lexer);
      continue;
    }

    if (feed.kind == TOK_LPAREN) {
      enum_kind->variant = ENUM_VARIANT_ALG;
      feed = parser_expect(TOK_IDENT, lexer_next(self->lexer));
      enum_kind->type.span = feed.span;
      parser_expect(TOK_RPAREN, lexer_next(self->lexer));
      parser_expect(TOK_COMMA, lexer_next(self->lexer));
    }
  }

  parser_expect(TOK_RBRACE, feed);

  ast_node.kind = AST_NODE_KIND_ENUM;
  ast_node.enum_case = enum_;

  debug("leaving enum\n");
  return ast_node;
}

AstNode parser_visit_import(Parser* self) {
  todo();
  xnotnull(self);
  AstNode ast_node = {};

  return ast_node;
}

Parser* parser_new(Allocator* alloc, Lexer* lexer) {
  Parser* self = allocator_alloc(alloc, sizeof(Parser));
  parser_init(self, alloc, lexer);
  return self;
}

void parser_init(Parser* self, Allocator* alloc, Lexer* lexer) {
  *self = (Parser){
      .lexer = lexer,
      .alloc = alloc,
  };
}
