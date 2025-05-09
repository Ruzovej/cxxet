#!/usr/bin/env bash

set -e

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nCompiling in "release" mode "rsm_dummy_app":\n\n'
time ./compile.bash --preset release -DRSM_BUILD_TESTS=ON
printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nExecuting compiled "rsm_dummy_app":\n\n'

export RSM_DEFAULT_BLOCK_SIZE=4 # torture it with some non-default value & force it to allocate more during the run
time {
    for i in {1..10}; do
        bin/release/rsm_dummy_app 0 "/tmp/easy_trace_${i}.json" --no-threads
    done
}
