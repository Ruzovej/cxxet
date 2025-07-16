#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"
#load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output" # TODO most probably not needed ...

function setup_file() {
    # TODO set those or not?!
    #export CXXET_DEFAULT_BLOCK_SIZE=2
    #export CXXET_VERBOSE=1
    # TODO guard against non-release preset mode "${CXXET_PRESET}" - see `tests/integration/bats_tests_runner.bash` around line 50
    export TMP_RESULT_DIR="$(mktemp -d "${TMPDIR:-/tmp}/cxxet.cmake_fetch_cxxet.bats.XXXXXX")"
}

function setup() {
    if [[ "${CXXET_PRESET:-release}" != 'release' ]]; then
        skip "this should test only 'release' build(s), current preset is '${CXXET_PRESET}'"
    fi
}

function teardown() {
    :
}

function teardown_file() {
    rm -rf "${TMP_RESULT_DIR}"
    #user_log "# results from this run are in '%s'\n" "${TMP_RESULT_DIR}"
}

@test "TODO_1" {
    :
    # TODO see `examples/cmake_fetch_content/CMakeLists.txt` and the comment here ...
}

@test "TODO_2" {
    :
    # TODO see `examples/cmake_fetch_content/CMakeLists.txt` and the comment here ...
}
