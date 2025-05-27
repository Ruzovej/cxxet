#!/usr/bin/env bash

set -e

printf -- '-=-=-=-=-=-=-=-=-=-=-=-\nExecuting predefined "bats" tests:\n\n'
time ./tests.bash --bats

printf -- '-=-=-=-=-=-=-=-=-=-=-=-\nExecuting predefined unit tests:\n\n'
time ./tests.bash --unit

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nCompiling in "release" mode "rsm_dummy_app":\n\n'
time ./compile.bash --preset release -DRSM_BUILD_TESTS=ON
printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nExecuting compiled "rsm_dummy_app":\n\n'

set -x

export RSM_DEFAULT_BLOCK_SIZE=5 # torture it with some non-default value & force it to allocate more during the run
time bin/release/rsm_dummy_app
time bin/release/rsm_dummy_app 0 /tmp/easy_trace.json
