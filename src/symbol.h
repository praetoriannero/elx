#pragma once

#include "str.h"

typedef enum {
  SYMBOL_KIND_VAR,
  SYMBOL_KIND_FUNC,
  SYMBOL_KIND_ENUM,
  SYMBOL_KIND_STRUCT,
  SYMBOL_KIND_TRAIT,
  SYMBOL_KIND_MODULE,
} SymbolKind;


typedef struct {
  SymbolKind kind;
  String name;
} Symbol;
