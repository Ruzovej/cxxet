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

    local preset
    for preset in "${test_presets[@]}"; do
        printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n' >&2
        printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for unit tests:\n' "${preset}" >&2
        compile \
            --preset "${preset}" \
            --target cxxet_unit_tests \
            --polite-ln-compile_commands >&2

        printf -- '-=-=-=-=-=-=-=- Executing %s unit tests:\n' "${preset}" >&2
        "bin/${preset}/cxxet_unit_tests" \
            --no-intro >&2

        printf '\n' >&2
    done
}
