#!/usr/bin/env bash

set -e

preset="${1:-asan_d}"

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nCompiling "rsm_dummy_app" in "%s" mode:\n\n' "${preset}"
time ./compile.bash \
    --preset "${preset}" \
    -DRSM_BUILD_TESTS=ON \
    --target rsm_dummy_app
printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nExecuting compiled "rsm_dummy_app" within gdb:\n\n'

set -x

export RSM_DEFAULT_BLOCK_SIZE=4 # torture it with some non-default value & force it to allocate more during the run
gdb --args "bin/${preset}/rsm_dummy_app"
