#!/usr/bin/env bash

set -e

cxxet_include scripts/common/reject_further_args

function format_cxx() {
    cxxet_require_command clang-format

    local format_folders=(
        "${CXXET_ROOT_DIR}/examples"
        "${CXXET_ROOT_DIR}/include"
        "${CXXET_ROOT_DIR}/src"
        "${CXXET_ROOT_DIR}/tests"
    )

    function usage() {
        if [[ "$1" != '--short' ]]; then
            printf 'format_cxx: formats all C/C++ source files in %s\n' >&2 "${format_folders[*]}"
        fi
        printf 'Usage: format_cxx [options...]\n' >&2
        printf 'Where options are:\n' >&2
        printf '    --help, -h    Show this help message\n' >&2
    }

    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        usage
        exit 0
    fi

    reject_further_args "$@" || {
        usage --short
        exit 1
    }

    find \
        "${format_folders[@]}" \
        \( -name "*.cxx" -o -name "*.hxx" -o -name "*.c" -o -name "*.h" \) \
        -type f \
        -exec \
            clang-format -i {} \;
}
