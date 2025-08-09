#!/usr/bin/env bash

set -e

function compile() {
    cxxet_require_command cmake

    local cxxet_preset=release
    local targets=()
    local defines=()
    local force_compile_commands_symlink='true'

    while (( $# > 0 )); do
        case "$1" in
            --preset|-p)
                cxxet_preset="${2:?No preset specified!}"
                shift 2
                ;;
            --target|-t)
                targets+=("$1" "${2:?No target specified!}")
                shift 2
                ;;
            -D*)
                defines+=("$1")
                shift 1
                ;;
            --polite-ln-compile_commands)
                force_compile_commands_symlink='false'
                shift 1
                ;;
            *)
                printf 'Unknown option: %s\n' "$1"
                exit 1
                ;;
        esac
    done

    local num_jobs="$(nproc)"
    (
        set -x
        cmake \
            -S "${CXXET_ROOT_DIR}" \
            -B "${CXXET_ROOT_DIR}/build/${cxxet_preset}" \
            "${defines[@]}" \
            --preset "${cxxet_preset}" #\
            # --graphviz="graphviz/${cxxet_preset}"
    )

    [[ "${force_compile_commands_symlink}" == 'false' && -f compile_commands.json ]] \
    || (
        set -x
        ln \
            --symbolic \
            --force \
            "${CXXET_ROOT_DIR}/build/${cxxet_preset}/compile_commands.json" \
            "${CXXET_ROOT_DIR}/compile_commands.json"
    )

    (
        set -x
        cmake \
            --build "${CXXET_ROOT_DIR}/build/${cxxet_preset}" \
            -j "${num_jobs}" \
            "${targets[@]}"
    )
}
