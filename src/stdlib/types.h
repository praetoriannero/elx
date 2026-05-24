#pragma once

typedef enum {
  NUM_BIN_OP_ADD,
  NUM_BIN_OP_SUB,
  NUM_BIN_OP_MUL,
  NUM_BIN_OP_DIV,
  NUM_BIN_OP_MOD,
} NumericBinaryOperator;

typedef enum {
  NUM_UN_OP_POS,
  NUM_UN_OP_NEG,
  NUM_UN_OP_INV,
  NUM_UN_OP_NOT,
} NumericUnaryOperator;

typedef enum {
  NUM_OP_UNARY,
  NUM_OP_BINARY,
} NumericOperatorKind;

typedef enum {
  OP_KIND_NUMERIC,
} OperatorKind;

typedef struct {
  char* name;
  // Operator operators[];
} Type;
