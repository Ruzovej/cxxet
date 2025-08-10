#!/usr/bin/env bash

set -e

function compile() {
    cxxet_require_command cmake

    local cxxet_preset=release
    local targets=()
    local defines=()
    local force_compile_commands_symlink='true'
    local quiet='false'

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'compile: configure & execute build system for given preset, target(s), etc.'
            fi
            printf 'Usage: compile [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Set the CMake preset (default: %s)\n' "${cxxet_preset}"
            printf '    --target, -t TARGET        Add a build target (can be used multiple times, default: all)\n'
            printf '    -DVAR=VALUE                Pass extra define to CMake\n'
            printf '    --ignore-compile_commands  Don'\''t create compile_commands.json symlink (by default creates it)\n'
            printf '    --quiet, -q                Suppress standard output\n'
            printf '    --help, -h                 Show this help message\n'
        } >&2
    }

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
            --ignore-compile_commands)
                force_compile_commands_symlink='false'
                shift 1
                ;;
            --quiet|-q)
                quiet='true'
                shift 1
                ;;
            --help|-h)
                usage
                exit 0
                ;;
            *)
                printf 'Unknown option: %s\n' "$1" >&2
                usage --short
                exit 1
                ;;
        esac
    done

    function redirect_output() {
        if [[ "${quiet}" == 'true' ]]; then
            cat 1>/dev/null
        else
            cat >&2
        fi
    }

    set -o pipefail
    (
        set -x
        cmake \
            -S "${CXXET_ROOT_DIR}" \
            -B "${CXXET_ROOT_DIR}/build/${cxxet_preset}" \
            "${defines[@]}" \
            --preset "${cxxet_preset}" #\
            # --graphviz="graphviz/${cxxet_preset}"
    ) | redirect_output

    [[ "${force_compile_commands_symlink}" == 'false' && -f "${CXXET_ROOT_DIR}/compile_commands.json" ]] \
    || (
        set -x
        ln \
            --symbolic \
            --force \
            "${CXXET_ROOT_DIR}/build/${cxxet_preset}/compile_commands.json" \
            "${CXXET_ROOT_DIR}/compile_commands.json"
    ) | redirect_output

    local num_jobs="$(nproc)"
    (
        set -x
        cmake \
            --build "${CXXET_ROOT_DIR}/build/${cxxet_preset}" \
            -j "${num_jobs}" \
            "${targets[@]}"
    ) | redirect_output

    set +o pipefail
}
