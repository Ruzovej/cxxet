#!/usr/bin/env bash

function bats_tests_runner() {
    local num_rounds=1

    while (( $# > 0 )); do
        case "$1" in
            -r|--rounds)
                shift
                num_rounds="$1"
                ;;
            *)
                break
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

    local test_presets=(
        asan_d
        asan
        tsan_d
        tsan
        release
    )

    local round=1
    while (( round <= num_rounds )); do
        [[ "${num_rounds}" -eq 1 ]] \
            || printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Executing bats tests round no. %s/%s:\n\n' "${round}" "${num_rounds}"
        (( round++ ))
        local preset
        for preset in "${test_presets[@]}"; do
            CXXST_PRESET="${preset}" \
            "${BATS_EXECUTABLE}" "${args[@]}" --recursive tests/integration/suite
        done
    done
}
