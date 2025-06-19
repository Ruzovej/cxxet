#!/usr/bin/env bash

set -e

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nCompiling examples in "release" mode:\n\n'
time ./compile.bash --preset release -DCXXET_BUILD_EXAMPLES=ON -DCXXET_BUILD_TESTS=ON
printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nExecuting compiled examples:\n\n'

printf -- '-=-=-=-=-=-=-=-=-=-=-=-\nExecuting predefined unit tests:\n\n'
time ./tests.bash --unit

printf -- '-=-=-=-=-=-=-=-=-=-=-=-\nExecuting predefined "bats" tests:\n\n'
time ./tests.bash --bats

set -x

export CXXET_DEFAULT_BLOCK_SIZE=3 # torture it with some non-default value & force it to allocate more during the run
time bin/release/cxxet_example_complete_1 /tmp/easy_complete_1.json
time bin/release/cxxet_example_counter_1 /tmp/easy_counter_1.json
time bin/release/cxxet_example_counter_2 /tmp/easy_counter_2.json
time bin/release/cxxet_example_duration_1 /tmp/easy_duration_1.json
time bin/release/cxxet_example_instant_1 /tmp/easy_instant_1.json
time bin/release/cxxet_example_instant_2 /tmp/easy_instant_2.json
