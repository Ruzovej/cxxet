#!/usr/bin/env bash

set -e

function format_cxx() {
    cxxet_require_command clang-format

    find \
        "${CXXET_ROOT_DIR}/examples" "${CXXET_ROOT_DIR}/include" "${CXXET_ROOT_DIR}/src" "${CXXET_ROOT_DIR}/tests" \
        \( -name "*.cxx" -o -name "*.hxx" -o -name "*.c" -o -name "*.h" \) \
        -type f \
        -exec \
            clang-format -i {} \;
}
