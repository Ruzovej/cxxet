#!/usr/bin/env bash

set -e

cxxet_include scripts/common/reject_further_args

function purge_artifacts() {
    # TODO (https://github.com/Ruzovej/cxxet/issues/111) is it enough? Or even more ... e.g. everything that is listed in (any) `.gitignore`?!
    local files_to_remove=(
        "${CXXET_ROOT_DIR}/bin"
        "${CXXET_ROOT_DIR}/build"
        "${CXXET_ROOT_DIR}/compile_commands.json"
        "${CXXET_ROOT_DIR}/tests/integration/external"
        "${CXXET_ROOT_DIR}/graphviz"
        "${CXXET_ROOT_DIR}/tmp"
    )

    function usage() {
        if [[ "$1" != '--short' ]]; then
            printf 'purge_artifacts: simply removes all folders/files: %s\n' >&2 "${files_to_remove[*]}"
        fi
        printf 'Usage: purge_artifacts [options...]\n' >&2
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
        
    rm -rf "${files_to_remove[@]}"
}
