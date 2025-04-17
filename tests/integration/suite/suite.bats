#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"

function user_log() {
    local fmt_string="$1"
    shift
    printf "# ${fmt_string}" "$@" >&3
}

function some_function() {
    local msg="${1:-default message}"
    local ret_code="${2:-0}"

    printf '%s\n' "${msg}"
    return "${ret_code}"
}

function setup_file() {
    user_log "configuring and building with preset '%s' ...\n" "${RSM_PRESET}"
    cmake -S . -B "build/${RSM_PRESET}" --preset "${RSM_PRESET}" -DRSM_BUILD_TESTS=ON
    cmake --build "build/${RSM_PRESET}" -j "$(nproc)" --target rsm_dummy_app
    user_log 'done ...\n'
    export RSM_TESTS_BINARY="bin/${RSM_PRESET}/rsm_dummy_app"
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
    user_log 'doing %s stuff ...\n' '1st'
    run some_function 'doing 1st stuff ...'
    assert_success
    assert_output --partial '1st'
}

@test "second" {
    user_log 'doing 2nd stuff ...\n'
    run some_function '2nd' 13
    assert_failure 13
    assert_output '2nd'
}
