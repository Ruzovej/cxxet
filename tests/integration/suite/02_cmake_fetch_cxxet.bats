#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"
#load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output" # TODO most probably not needed ...

function setup_file() {
    if [[ "${CXXET_PRESET:-release}" != 'release' ]]; then
        skip "this should test only 'release' build(s), current preset is '${CXXET_PRESET}'"
    fi

    # TODO set those or not?!
    #export CXXET_DEFAULT_BLOCK_SIZE=2
    #export CXXET_VERBOSE=1
    export TMP_RESULT_DIR="$(mktemp -d "${TMPDIR:-/tmp}/cxxet.02_cmake_fetch_cxxet.bats.XXXXXX")"

    user_log "# using tmp dir '%s', repository in '%s' and testing its commit '%s'\n" \
        "${TMP_RESULT_DIR}" "${CXXET_PWD:?}" "${CXXET_CURRENT_COMMIT_HASH:?}"
    if [[ -n "${CXXET_UNCOMMITED_CHANGES}" ]]; then
        user_log "\n# !!! BUT BEWARE, THERE ARE UNCOMMITTED CHANGES THAT WON'T BE TESTED !!!\n\n"
    fi
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    if [[ -n "${TMP_RESULT_DIR}" ]]; then
        rm -rf "${TMP_RESULT_DIR}"
        user_log '# used tmp dir erased\n'
    fi
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
