#!/bin/bash

git ls-files "*.h" "*.c" | xargs clang-format -i --style="{BasedOnStyle: llvm, IndentWidth: 4, PointerAlignment: Left, ColumnLimit: 120}" --verbose

