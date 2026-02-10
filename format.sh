#!/bin/bash

git ls-files "*.h" "*.c" | xargs clang-format -i --style="{BasedOnStyle: llvm, IndentWidth: 4}" --verbose

