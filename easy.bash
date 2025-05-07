#!/usr/bin/env bash

set -e

printf -- '-=-=-=-=-=-=-=-=-=-=-=-\nExecuting predefined "bats" tests:\n\n'
time ./tests.bash --bats

printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nCompiling in "release" mode "rsm_dummy_app":\n\n'
time ./compile.bash --preset release -DRSM_BUILD_TESTS=ON
printf '\n-=-=-=-=-=-=-=-=-=-=-=-\nExecuting compiled "rsm_dummy_app":\n\n'
time bin/release/rsm_dummy_app
