#!/usr/bin/env bash

function bats_runner() {
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
        asan
        asan_d
        tsan
        tsan_d
        release
    )

    local round=1
    while (( round <= num_rounds )); do
        [[ "${num_rounds}" -eq 1 ]] \
            || printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\nExecuting bats tests round no. %s/%s:\n\n' "${round}" "${num_rounds}"
        (( round++ ))
        local preset
        for preset in "${test_presets[@]}"; do
            RSM_PRESET="${preset}" \
            "${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/suite.bats
        done
    done
}
