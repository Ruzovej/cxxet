#!/usr/bin/env bash

function bats_tests_runner() {
    local test_presets=(
        asan_d
        asan
        tsan_d
        tsan
        release
    )

    while (($# > 0)); do
        case "$1" in
        -p | --preset)
            shift
            if [[ -z "$1" ]]; then
                echo "Error: No preset specified after -p/--preset option."
                return 1
            fi
            test_presets=("$1")
            ;;
        *)
            printf 'Unknown option(s): "%s"\n' "$@" >&2
            exit 1
            ;;
        esac
        shift
    done

    source tests/integration/init/initialize_bats.bash
    initialize_bats

    local args=(
        --timing
        #--tap
    )

    local preset
    for preset in "${test_presets[@]}"; do
        printf 'Running bats tests with preset %s:\n' "${preset}"
        (
            export CXXET_PRESET="${preset}"
            export CXXET_PWD="${PWD}"
            export CXXET_CURRENT_COMMIT_HASH="$(git -C "${CXXET_PWD}" rev-parse HEAD)"
            export CXXET_UNCOMMITED_CHANGES="$(git -C "${CXXET_PWD}" status --porcelain)"
            "${BATS_EXECUTABLE}" "${args[@]}" --recursive tests/integration/suite
            #"${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/01_suite.bats
            #"${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/02_cmake_fetch_cxxet.bats
        )
    done
}
