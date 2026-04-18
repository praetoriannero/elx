#!/bin/bash

git ls-files "*.h" "*.c" | xargs clang-format -i --style="{BasedOnStyle: llvm, IndentWidth: 2, PointerAlignment: Left, ColumnLimit: 80, IndentCaseLabels: true}" --verbose
