#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function unit_runner() {
    local test_presets=(
        asan
        asan_d
        tsan
        tsan_d
        release
    )

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'unit_runner: run unit tests with specified preset(s)\n'
            fi
            printf 'Usage: tests unit [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Run tests only for the specified preset (default is all presets: %s)\n' "${test_presets[*]}"
            printf '    --help, -h                 Show this help message\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            -p|--preset)
                test_presets=("${2:?No preset specified!}")
                shift 2
                ;;
            --help|-h)
                usage
                return 0
                ;;
            *)
                printf 'Unknown option: %s\n' "$1" >&2
                usage --short
                exit 1
                ;;
        esac
    done

    local preset
    for preset in "${test_presets[@]}"; do
        printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for unit tests:\n' "${preset}" >&2
        compile \
            --preset "${preset}" \
            --target cxxet_unit_tests \
            --ignore-compile_commands >&2

        printf -- '-=-=-=-=-=-=-=- Executing %s unit tests:\n' "${preset}" >&2
        "${CXXET_ROOT_DIR}/bin/${preset}/cxxet_unit_tests" \
            --no-intro >&2

        printf '\n' >&2
    done
}
