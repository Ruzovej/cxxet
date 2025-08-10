#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function unit_runner() {
    local arg_preset
    while (( $# > 0 )); do
        case "$1" in
            -p|--preset)
                arg_preset="${2:?No preset specified!}"
                shift 2
                ;;
            *)
                printf 'Unknown option: %s\n' "$1" >&2
                exit 1
                ;;
        esac
    done

    local test_presets
    if [[ -z "${arg_preset}" ]]; then
        test_presets=(
            asan
            asan_d
            tsan
            tsan_d
            release
        )
    else
        test_presets=("${arg_preset}")
    fi

    local preset
    for preset in "${test_presets[@]}"; do
        printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n' >&2
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
