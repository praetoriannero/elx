#!/bin/bash

git ls-files "*.h" "*.c" | xargs clang-format -i --verbose
