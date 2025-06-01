#!/usr/bin/env bash

set -e

preset="${1:-asan_d}"
executable="${2:-rsm_dummy_app}"

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nCompiling "%s" in "%s" mode:\n\n' "${executable}" "${preset}"
time ./compile.bash \
    --preset "${preset}" \
    -DRSM_BUILD_TESTS=ON \
    --target "${executable}"

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nExecuting compiled "%s" within gdb:\n\n' "${executable}"
set -x
export RSM_DEFAULT_BLOCK_SIZE="${3:-4}" # torture it with some non-default value & force it to allocate more during the run
gdb --args "bin/${preset}/${executable}"
