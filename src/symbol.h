#pragma once

#include <stdbool.h>

#include "parser.h"
#include "strview.h"
#include "vector.h"

typedef enum {
  SYMBOL_KIND_VAR,
  SYMBOL_KIND_FUNC,
  SYMBOL_KIND_ENUM,
  SYMBOL_KIND_STRUCT,
  SYMBOL_KIND_TRAIT,
  SYMBOL_KIND_MODULE,
} SymbolKind;

typedef struct {
  StringView name;
  bool public;
  SymbolKind kind;
  union {
    struct {
      Type type;
      bool mutable;
    } var;

    struct {
      Type return_type;
    } func;

    struct {
      Vector cases;
    } enum_;

    struct {
      Type type;
    } struct_;
  };
} Symbol;
