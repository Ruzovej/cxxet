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
    ./compile.bash \
        -DRSM_BUILD_TESTS=ON \
        --preset "${RSM_PRESET}" \
        --target rsm_dummy_app \
        --target rsm_infra_sanitizer_check \
        --polite-ln-compile_commands # 2>&3 1>&3 # TODO use or delete? This displays the output of it in console ...
    user_log 'done ...\n'
    export RSM_BIN_DIR="bin/${RSM_PRESET}"
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

@test "sanitizers work as expected" {
    local san_check="${RSM_BIN_DIR}/rsm_infra_sanitizer_check"
    if [[ "${RSM_PRESET}" =~ asan* ]]; then
        run "${san_check}" asan
        assert_failure
        assert_output --partial "runtime error: index 2 out of bounds for type 'int [2]'"

        run "${san_check}" lsan
        assert_failure
        assert_output --partial "==ERROR: LeakSanitizer: detected memory leaks"
        assert_output --partial "SUMMARY: AddressSanitizer: 8 byte(s) leaked in 2 allocation(s)."

        run "${san_check}" ubsan
        assert_failure
        assert_output --partial 'runtime error: left shift of negative value -1'
    elif [[ "${RSM_PRESET}" =~ tsan* ]]; then
        run "${san_check}" tsan
        assert_failure
        assert_output --partial 'WARNING: ThreadSanitizer: data race'
        assert_output --partial 'ThreadSanitizer: reported 1 warnings'

        run "${san_check}" ubsan
        assert_failure
        assert_output --partial 'runtime error: left shift of negative value -1'
    else
        run "${san_check}" tsan
        assert_success
        assert_output ''

        run "${san_check}" ubsan
        assert_success
        assert_output ''

        run "${san_check}" asan
        assert_success
        assert_output ''

        run "${san_check}" lsan
        assert_success
        assert_output ''
    fi
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
