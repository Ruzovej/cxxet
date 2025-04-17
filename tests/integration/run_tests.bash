#!/usr/bin/env bash

set -e

source tests/integration/init/initialize_bats.bash
initialize_bats

args=(
    --timing
    #--tap
)

"${BATS_EXECUTABLE}" "${args[@]}" tests/integration/suite/suite.bats
