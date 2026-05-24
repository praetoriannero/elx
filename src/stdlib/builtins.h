#pragma once

static const char* INTEGER_BINARY_METHODS[] = {
    "__add__", // +
    "__sub__", // -
    "__div__", // /
    "__mul__", // *
    "__exp__", // **
    "__mod__", // %
    "__xor__", // ^
    "__or__",  // |
    "__and__", // &
    "__shl__", // <<
    "__shr__", // >>
    "__eq__",  // ==
    "__ne__",  // !=
};

static const char* INTEGER_UNARY_METHODS[] = {
    "__str__", // $
    "__inv__", // ~
    "__neg__", // -
    "__pos__", // +
    "__not__", // !
};

typedef struct {
  char* name;
} BuiltinType;
