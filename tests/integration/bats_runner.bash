#!/usr/bin/env bash

function bats_runner() {
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

    local preset
    for preset in "${test_presets[@]}"; do
        RSM_PRESET="${preset}" \
        "${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/suite.bats
    done
}
