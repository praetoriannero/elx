#include <assert.h>
#include <string.h>

#include "core/panic.h"
#include "core/todo.h"
#include "core/vector.h"
#include "core/xalloc.h"
#include "parser/parser.h"
#include "parser_utils.h"

const char INDENT[] = "  ";

void indent(u64 depth) {
  for (u64 i = 0; i < depth; i++) {
    printf("%s", INDENT);
  }
}

void print_global(Global* global, u64 depth) {
  indent(depth);
  printf("global {\n");
  depth++;
  printf("ident: %s,\n", global->name);
  printf("mut: %i,\n", global->mut);
  print_expr(global->expr, depth + 1);
  depth--;
  printf("},\n");
}

void print_func(Func* func, u64 depth) {
  indent(depth);
  printf("func {\n");
  depth++;
  indent(depth);
  printf("ident: %s,\n", func->name);
  indent(depth);
  printf("params {\n");
  depth++;
  if (func->param_vec.size) {
    FuncArg* func_arg = NULL;
    vector_foreach(func_arg, func->param_vec) {
      indent(depth);
      printf("param {%s: %s},\n", func_arg->name, func_arg->type.name);
    }
  }
  depth--;
  indent(depth);
  printf("},\n");
  indent(depth);
  printf("return_type: %s,\n", func->ret_type.name);
  indent(depth);
  printf("body {\n");
  print_body(&func->body, depth);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("}\n");
}

void print_assign_stmt(AssignStmt* assign_stmt, u64 depth) {
  indent(depth);
  printf("assign_stmt {\n");
  depth++;
  indent(depth);
  printf("ident: %s,\n", assign_stmt->name);
  indent(depth);
  printf("mut: %i\n", assign_stmt->mut);
  indent(depth);
  printf("expr {\n");
  print_expr(assign_stmt->expr, depth + 1);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("},\n");
}

void print_return_stmt(ReturnStmt* return_stmt, u64 depth) {
  indent(depth);
  printf("return_stmt {\n");
  depth++;
  indent(depth);
  printf("expr {\n");
  print_expr(return_stmt->expr, depth + 1);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("},\n");
}

void print_expr_stmt(ExprStmt* expr_stmt, u64 depth) {
  indent(depth);
  printf("expr_stmt {\n");
  depth++;
  indent(depth);
  printf("expr {\n");
  print_expr(expr_stmt->expr, depth + 1);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("},\n");
}

void print_for_stmt(ForStmt* for_stmt, u64 depth) {
  indent(depth);
  printf("for_stmt {\n");
  depth++;
  indent(depth);
  printf("iterator: %s,\n", for_stmt->iterator);
  indent(depth);
  printf("iterable {\n");
  print_expr(for_stmt->iterable, depth + 1);
  indent(depth);
  printf("},\n");
  indent(depth);
  printf("body {\n");
  print_body(&for_stmt->body, depth);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("},\n");
}

void print_while_stmt(WhileStmt* while_stmt, u64 depth) {
  indent(depth);
  printf("while_stmt {\n");
  depth++;
  indent(depth);
  printf("condition {\n");
  print_expr(while_stmt->condition, depth + 1);
  indent(depth);
  printf("},\n");
  indent(depth);
  printf("body {\n");
  print_body(&while_stmt->body, depth);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("},\n");
}

void print_elif_clause(ElifClause* elif_clause, u64 depth) {
  indent(depth);
  printf("elif_clause {\n");
  depth++;
  indent(depth);
  printf("condition {\n");
  print_expr(elif_clause->condition, depth);
  indent(depth);
  printf("},\n");
  indent(depth);
  printf("body {\n");
  print_body(&elif_clause->body, depth);
  indent(depth);
  printf("},\n");
  depth--;
  indent(depth);
  printf("},\n");
}

void print_else_clause(ElseClause* else_clause, u64 depth) {
  indent(depth);
  printf("else_clause {\n");
  printf("body {\n");
  print_body(&else_clause->body, depth);
  printf("},\n");
  indent(depth);
  printf("},\n");
}

void print_if_stmt(IfStmt* if_stmt, u64 depth) {
  indent(depth);
  printf("if_stmt {\n");
  depth++;
  indent(depth);
  printf("condition {\n");
  print_expr(if_stmt->condition, depth);
  indent(depth);
  printf("},\n");
  indent(depth);
  printf("body {\n");
  print_body(&if_stmt->body, depth);
  indent(depth);
  printf("},\n");
  ElifClause* elif_clause = NULL;
  vector_foreach(elif_clause, if_stmt->elif_clause_vec) { print_elif_clause(elif_clause, depth); }
  depth--;
  indent(depth);
  printf("},\n");
}

void print_break_stmt(BreakStmt* break_stmt, u64 depth) {
  indent(depth);
  printf("break,");
}

void print_continue_stmt(ContinueStmt* continue_stmt, u64 depth) {
  indent(depth);
  printf("continue,");
}

void print_body(Body* body, u64 depth) {
  Stmt* stmt = NULL;
  vector_foreach(stmt, body->stmts) {
    switch (stmt->kind) {
      case STMT_KIND_ASSIGN:
        print_assign_stmt(&stmt->assign_stmt, depth + 1);
        break;
      case STMT_KIND_RETURN:
        print_return_stmt(&stmt->return_stmt, depth + 1);
        break;
      case STMT_KIND_EXPR:
        print_expr_stmt(&stmt->expr_stmt, depth + 1);
        break;
      case STMT_KIND_FOR:
        print_for_stmt(&stmt->for_stmt, depth + 1);
        break;
      case STMT_KIND_WHILE:
        print_while_stmt(&stmt->while_stmt, depth + 1);
        break;
      case STMT_KIND_IF:
        print_if_stmt(&stmt->if_stmt, depth + 1);
        break;
      case STMT_KIND_BREAK:
        print_break_stmt(&stmt->break_stmt, depth + 1);
        break;
      case STMT_KIND_CONTINUE:
        print_continue_stmt(&stmt->cont_stmt, depth + 1);
        break;
      case STMT_KIND_UNDEFINED:
        panic("undefined statement kind\n");
    }
  }
}

void print_module(Module* module, u64 depth) {
  indent(depth);
  printf("module {\n");
  AstNode* ast_node = NULL;
  vector_foreach(ast_node, module->ast_node_vec) {
    switch (ast_node->kind) {
      case AST_NODE_KIND_MODULE:
        print_module(&ast_node->module_case, depth + 1);
        continue;
      case AST_NODE_KIND_ENUM:
        print_enum(&ast_node->enum_case, depth + 1);
        continue;
      case AST_NODE_KIND_FUNC:
        print_func(&ast_node->func_case, depth + 1);
        continue;
      case AST_NODE_KIND_GLOBAL:
        print_global(&ast_node->global_case, depth + 1);
        continue;
      case AST_NODE_KIND_STRUCT:
        print_struct(&ast_node->struct_case, depth + 1);
        continue;
      // case SYMBOL_KIND_IMPORT:
      //     print_import(&symbol->import_case, depth + 1);
      //     continue;
      case AST_NODE_KIND_UNDEFINED:
        break;
    }
  }
  // depth--;
  printf("}\n");
}

void print_struct(Struct* struct_, u64 depth) {
  indent(depth);
  printf("struct {\n");
  depth++;
  indent(depth);
  printf("ident: %s,\n", struct_->name);
  for (u32 i = 0; i < struct_->field_vec.size; i++) {
    indent(depth);
    // StructField* field = vector_get(&struct_->field_vec, i);
    StructField* field = vector_get(&struct_->field_vec, StructField, i);
    // xnotnull(field);
    printf("field { ident: %s, type: %s },\n", field->name, field->type.name);
  }
  // for (usize j = 0; j < struct_->method_vec.size; j++) {
  //   AstNode* ast_node = vector_get(&struct_->method_vec, j);
  AstNode* ast_node = NULL;
  vector_foreach(ast_node, struct_->method_vec) { print_func(&ast_node->func_case, depth); }
  depth--;
  indent(depth);
  printf("},\n");
}

u64 count_csv(const char* data, char sep) {
  // count the comma separated values before finding the right-hand version of
  // the separator todo: better string handling to allow regex
  assert(data != NULL);

  u64 count = 0;
  u64 arr[256] = {
      ['{'] = 0,
      ['('] = 0,
      ['['] = 0,
      ['"'] = 0,
  };
  u64 translate[256] = {
      ['}'] = '{',
      [')'] = '(',
      [']'] = '[',
      ['"'] = '"',
  };

  arr[(u8)sep]++;
  usize data_len = strlen(data);
  for (usize i = 0; i < data_len; i++) {
    if (data[i] == '"') {
      for (usize j = i + 1; j < data_len; j++) {
        if (!data[j]) {
          panic("unexpected EOF during expression parse\n");
        }

        if (data[j] == '"') {
          i = j;
          break;
        }
      }
    }
    if (strchr("{([", data[i])) {
      arr[(u8)data[i]]++;
    }

    if (strchr("})]", data[i])) {
      arr[(u8)data[translate[i]]]--;
    }

    u64 arr_sum = 0;
    for (size_t j = 0; j < 256; j++) {
      arr_sum += arr[j];
    }

    if ((arr_sum == 1) && (data[i] == ',')) {
      count++;
    }

    if (!arr_sum) {
      break;
    }
  }

  return count;
}

void print_enum(Enum* enum_, u64 depth) { todo(); }

void print_import(Import* import, u64 depth) { todo(); }

void print_ast(Ast* ast) {
  u64 depth = 0;
  // for (usize i = 0; i < ast->module_vec.size; i++) {
  //   Module* module = vector_get(&ast->module_vec, i);
  Module* module = NULL;
  vector_foreach(module, ast->module_vec) { print_module(module, depth); }
}

void print_expr(Expr* expr, u64 depth) {
  xnotnull(expr);
  Expr* arg_expr = NULL;
  usize vec_len;

  indent(depth);
  switch (expr->expr_kind) {
    case EXPR_KIND_METHOD_CALL:
      printf("method call {\n");
      depth++;
      indent(depth);
      printf("object {\n");
      print_expr(expr->method_call_expr.object, depth + 1);
      indent(depth);
      printf("},\n");
      indent(depth);
      printf("field: %s,\n", expr->method_call_expr.method);
      vec_len = expr->method_call_expr.arg_vec.size;
      indent(depth);
      printf("args {\n");
      // for (usize i = 0; i < vec_len; i++) {
      //   Expr* arg_expr = vector_get(&expr->method_call_expr.arg_vec, i);
      // Expr* arg_expr = NULL;
      vector_foreach(arg_expr, expr->method_call_expr.arg_vec) { print_expr(arg_expr, depth + 1); }
      indent(depth);
      printf("},\n");
      depth--;

      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_EMPTY:
      printf("<EMPTY>\n");
      break;

    case EXPR_KIND_IDENT:
      printf("ident: %s,\n", expr->ident_expr);
      break;

    case EXPR_KIND_LITERAL:
      switch (expr->literal_expr.kind) {
        case LITERAL_KIND_STRING:
          printf("string literal: '%s',\n", expr->literal_expr.string);
          break;

        case LITERAL_KIND_FLOAT:
          printf("float literal: '%s',\n", expr->literal_expr.float_);
          break;

        case LITERAL_KIND_INTEGER:
          printf("integer literal: '%s',\n", expr->literal_expr.integer);
          break;

        default:
          printf("<UNSUPPORTED_LITERAL>\n");
          break;
      }
      break;

    case EXPR_KIND_ARRAY_IMPLICIT:
      printf("array implicit {\n");
      print_expr(expr->array_implicit_expr.kind, depth + 1);
      print_expr(expr->array_implicit_expr.size, depth + 1);
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_ARRAY_EXPLICIT:
      printf("array explicit {\n");
      vec_len = expr->array_explicit_expr.arg_vec.size;
      // for (usize i = 0; i < vec_len; i++) {
      //   Expr* arg_expr = vector_get(&expr->array_explicit_expr.arg_vec, i);
      // Expr* arg_expr = NULL;
      // vector_foreach(arg_expr, expr->array_explicit_expr.arg_vec) {
      for (usize i = 0; i < vec_len; i++) {
        arg_expr = vector_get(&expr->array_explicit_expr.arg_vec, Expr, i);
        print_expr(arg_expr, depth + 1);
      }
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_BINARY:
      printf("binary %s {\n", expr->binary_expr.op.str.data);
      print_expr(expr->binary_expr.lhs, depth + 1);
      print_expr(expr->binary_expr.rhs, depth + 1);
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_UNARY:
      printf("unary %s {\n", expr->unary_expr.op.str.data);
      print_expr(expr->unary_expr.inner, depth + 1);
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_ARRAY_INDEX:
      printf("array index {\n");
      print_expr(expr->array_index_expr.index, depth + 1);
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_STRUCT:
      printf("struct init {\n");
      depth++;
      indent(depth);
      printf("object {\n");
      print_expr(expr->struct_init_expr.object, depth + 1);
      indent(depth);
      printf("},\n");
      depth--;
      vec_len = expr->struct_init_expr.arg_vec.size;
      depth++;
      indent(depth);
      printf("args {\n");
      for (usize i = 0; i < vec_len; i++) {
        arg_expr = vector_get(&expr->struct_init_expr.arg_vec, Expr, i);
        print_expr(arg_expr, depth + 1);
      }
      indent(depth);
      printf("},\n");
      depth--;
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_FIELD:
      printf("field access {\n");
      depth++;
      indent(depth);
      printf("object {\n");
      print_expr(expr->field_expr.object, depth + 1);
      indent(depth);
      printf("},\n");
      indent(depth);
      printf("field: %s,\n", expr->field_expr.name);
      depth--;
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_PATH:
      printf("path {\n");
      depth++;
      indent(depth);
      printf("stem: %s,\n", expr->path_expr.stem);
      indent(depth);
      printf("leaf {\n");
      print_expr(expr->path_expr.expr, depth + 1);
      indent(depth);
      printf("},\n");
      depth--;
      indent(depth);
      printf("},\n");
      break;

    case EXPR_KIND_CALL:
      vec_len = expr->call_expr.arg_vec.size;
      printf("call {\n");
      depth++;
      indent(depth);
      printf("object {\n");
      print_expr(expr->call_expr.object, depth + 1);
      indent(depth);
      printf("},\n");
      indent(depth);
      printf("args {\n");
      depth++;
      for (usize i = 0; i < vec_len; i++) {
        indent(depth);
        printf("arg_%zu {\n", i);
        arg_expr = vector_get(&expr->call_expr.arg_vec, Expr, i);
        print_expr(arg_expr, depth + 1);
        indent(depth);
        printf("},\n");
      }
      depth--;
      indent(depth);
      printf("},\n");
      depth--;
      indent(depth);
      printf("},\n");
      break;

    default:
      printf("<UNSUPPORTED: %i>\n", expr->expr_kind);
      break;
  }
}

char* expr_kind_string(ExprKind kind) {
  switch (kind) {
      // EmptyExpression
    case EXPR_KIND_EMPTY:
      return "EXPR_KIND_EMPTY";

      // LiteralExpression
    case EXPR_KIND_LITERAL:
      return "EXPR_KIND_LITERAL";

      // PathExpression
    case EXPR_KIND_PATH:
      return "EXPR_KIND_PATH";

      // OperatorExpression
    case EXPR_KIND_BINARY:
      return "EXPR_KIND_BINARY";
    case EXPR_KIND_UNARY:
      return "EXPR_KIND_UNARY";

      // GroupedExpression
    case EXPR_KIND_GROUP:
      return "EXPR_KIND_GROUP";

      // ArrayExpression
    case EXPR_KIND_ARRAY_EXPLICIT:
      return "EXPR_KIND_ARRAY_EXPLICIT";
    case EXPR_KIND_ARRAY_IMPLICIT:
      return "EXPR_KIND_ARRAY_IMPLICIT";

      // IndexExpression
    case EXPR_KIND_ARRAY_INDEX:
      return "EXPR_KIND_ARRAY_INDEX";

      // TupleExpression
    case EXPR_KIND_TUPLE:
      return "EXPR_KIND_TUPLE";

      // note: this is really a field expression
      // TupleIndexingExpression
      // case EXPR_KIND_TUPLE_INDEX: return "EXPR_KIND_TUPLE_INDEX";

      // StructExpression
    case EXPR_KIND_STRUCT:
      return "EXPR_KIND_STRUCT";

      // CallExpression
    case EXPR_KIND_CALL:
      return "EXPR_KIND_CALL";

      // MethodCallExpression
    case EXPR_KIND_METHOD_CALL:
      return "EXPR_KIND_METHOD_CALL";

      // FieldExpression
    case EXPR_KIND_FIELD:
      return "EXPR_KIND_FIELD";

      // ClosureExpression
    case EXPR_KIND_CLOSURE:
      return "EXPR_KIND_CLOSURE";

      // RangeExpression
    case EXPR_KIND_RANGE:
      return "EXPR_KIND_RANGE";

      // AssignmentExpression
    case EXPR_KIND_ASSIGN:
      return "EXPR_KIND_ASSIGN";

      // future
      // AwaitExpression
      // AsyncBlockExpression
      // UnderscoreExpression

      // JumpExpression
      // case EXPR_KIND_RETURN: return "EXPR_KIND_RETURN";
      // case EXPR_KIND_BREAK: return "EXPR_KIND_BREAK";
      // case EXPR_KIND_CONTINUE: return "EXPR_KIND_CONTINUE";

    case EXPR_KIND_IDENT:
      return "EXPR_KIND_IDENT";
  }
}
