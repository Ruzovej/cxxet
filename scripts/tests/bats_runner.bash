#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile
cxxet_include scripts/tests/initialize_bats

function bats_runner() {
    cxxet_require_command \
        jq \
        strace \
        nm

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

    local args=(
        --timing
        #--tap
    )

    (
        initialize_bats

        export CXXET_PWD="${CXXET_ROOT_DIR}"
        export CXXET_CURRENT_COMMIT_HASH="$(git -C "${CXXET_PWD}" rev-parse HEAD)"
        export CXXET_UNCOMMITED_CHANGES="$(git -C "${CXXET_PWD}" status --porcelain)"

        local preset
        for preset in "${test_presets[@]}"; do
            export CXXET_PRESET="${preset}"

            printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n' >&2
            printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for bats tests:\n' "${preset}" >&2
            compile \
                --preset "${preset}" \
                --target infra_sanitizer_check \
                --target cxxet_examples \
                --target cxxet_unit_tests \
                --ignore-compile_commands >&2

            printf -- '-=-=-=-=-=-=-=- Executing %s bats tests:\n' "${preset}" >&2
            "${BATS_EXECUTABLE}" "${args[@]}" --recursive tests/integration/suite
            #"${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/01_suite.bats
            #"${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/02_cmake_fetch_cxxet_direct_usage.bats
            #"${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/03_cmake_fetch_cxxet_indirect_usage.bats

            printf '\n' >&2
        done
    )
}
