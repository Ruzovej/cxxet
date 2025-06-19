#!/usr/bin/env bash

function bats_tests_runner() {
    local num_rounds=1

    local test_presets=(
        asan_d
        asan
        tsan_d
        tsan
        release
    )

    while (( $# > 0 )); do
        case "$1" in
            -r|--rounds)
                shift
                num_rounds="$1"
                ;;
            -p|--preset)
                shift
                if [[ -z "$1" ]]; then
                    echo "Error: No preset specified after -p/--preset option."
                    return 1
                fi
                test_presets=("$1")
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

    local round=1
    while (( round <= num_rounds )); do
        [[ "${num_rounds}" -eq 1 ]] \
            || printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Executing bats tests round no. %s/%s:\n\n' "${round}" "${num_rounds}"
        (( round++ ))
        local preset
        for preset in "${test_presets[@]}"; do
            CXXET_PRESET="${preset}" \
            "${BATS_EXECUTABLE}" "${args[@]}" --recursive tests/integration/suite
        done
    done
}
