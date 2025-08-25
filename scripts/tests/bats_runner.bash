#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile
cxxet_include scripts/tests/initialize_bats

function bats_runner() {
    cxxet_require_command \
        jq \
        nm \
        parallel \
        strace

    local test_presets=(
        asan_d
        asan
        tsan_d
        tsan
        release
    )

    local no_parallel='false'

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'bats_runner: run bats integration tests with specified preset(s)\n'
            fi
            printf 'Usage: tests bats [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Run tests only for the specified preset (default is all presets: %s)\n' "${test_presets[*]}"
            printf '    --no-parallel              Disable parallel test execution\n'
            printf '    --help, -h                 Show this help message\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            -p|--preset)
                test_presets=("${2:?No preset specified!}")
                shift 2
                ;;
            --no-parallel)
                no_parallel='true'
                shift
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

    local args=(
        --timing
        --line-reference-format colon
        #--tap
    )

    if [[ "${no_parallel}" == 'false' ]]; then
        args+=(--jobs "$(nproc)")
    fi

    (
        initialize_bats

        export TMP_RESULT_DIR_BASE="${CXXET_ROOT_DIR}/tmp/$(date +%Y-%m-%dT%H-%M-%S)_bats"
        
        local reports_folder="${TMP_RESULT_DIR_BASE}/bats_reports"
        mkdir -p "${reports_folder}"
        args+=(
            --output "${reports_folder}"
        )

        export CXXET_CURRENT_COMMIT_HASH="$(git -C "${CXXET_ROOT_DIR}" rev-parse HEAD)"
        export CXXET_UNCOMMITED_CHANGES="$(git -C "${CXXET_ROOT_DIR}" status --porcelain)"

        local preset
        for preset in "${test_presets[@]}"; do
            printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for bats tests:\n' "${preset}" >&2
            compile \
                --quiet \
                --preset "${preset}" \
                --target infra_sanitizer_check \
                --target cxxet_examples \
                --target cxxet_unit_tests \
                --ignore-compile_commands >&2

            printf -- '-=-=-=-=-=-=-=- Executing %s bats tests:\n' "${preset}" >&2
            export CXXET_PRESET="${preset}"
            "${BATS_EXECUTABLE}" "${args[@]}" --recursive "${CXXET_ROOT_DIR}/tests/integration/suite"
            #"${BATS_EXECUTABLE}" "${args[@]}" "${CXXET_ROOT_DIR}/tests/integration/suite/01_suite.bats"
            #"${BATS_EXECUTABLE}" "${args[@]}" "${CXXET_ROOT_DIR}/tests/integration/suite/02_cmake_fetch_cxxet_direct_usage.bats"
            #"${BATS_EXECUTABLE}" "${args[@]}" "${CXXET_ROOT_DIR}/tests/integration/suite/03_cmake_fetch_cxxet_indirect_usage.bats"
            #"${BATS_EXECUTABLE}" --help

            printf '\n' >&2
        done
    )
}
