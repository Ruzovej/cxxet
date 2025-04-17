#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"

function user_log() {
    :
    #printf "$@" >&3
}

function some_function() {
    local msg="${1:-default message}"
    local ret_code="${2:-0}"

    printf '%s\n' "${msg}"
    return "${ret_code}"
}

function setup_file() {
    :
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    :
}

@test "first" {
    user_log '# doing %s stuff ...\n' '1st'
    run some_function 'doing 1st stuff ...'
    assert_success
    assert_output --partial '1st'
}

@test "second" {
    user_log '# doing 2nd stuff ...\n'
    run some_function '2nd' 13
    assert_failure 13
    assert_output '2nd'
}
