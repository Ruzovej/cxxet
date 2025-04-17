#!/usr/bin/env bash

set -e

source tests/integration/init/initialize_bats.bash
initialize_bats

args=(
    --timing
    #--tap
)

test_presets=(
    tsan
    tsan_d
    asan
    asan_d
    release
)

for preset in "${test_presets[@]}"; do
    RSM_PRESET="${preset}" \
    "${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/suite.bats
done
