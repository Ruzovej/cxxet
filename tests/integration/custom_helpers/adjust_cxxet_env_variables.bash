#!/usr/bin/env bash

function adjust_cxxet_env_variables() {
    # vast majority of tests check even output implied by this option
    export CXXET_VERBOSE="${1:-1}"
    # by default, torture it with some non-default & sub-optimal value in order to (hopefully) discover any hidden issues hidden during "normal" usage:
    export CXXET_DEFAULT_BLOCK_SIZE="${2:-2}"
    # by default, disable dumping events into "implicit" file (via keeping this empty):
    export CXXET_TARGET_FILENAME="$3"
}
