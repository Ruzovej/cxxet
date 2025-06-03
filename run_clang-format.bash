#!/bin/bash

set -e

if ! command -v clang-format &> /dev/null; then
    printf "clang-format not found!\n" >&2
    exit 1
fi

find \
    examples include src tests \
    \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) \
    -type f \
    -exec \
        clang-format -i {} \;
