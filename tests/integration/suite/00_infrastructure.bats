#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_CXXET_BIN_DIR"

function setup_file() {
    populate_CXXET_BIN_DIR
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

# TODO extract the binary from `CMakeLists.txt` to it's own, standalone one
@test "Sanitizers work as expected" {
    local san_check="${CXXET_BIN_DIR}/infra_sanitizer_check"
    if [[ "${CXXET_PRESET}" =~ asan* ]]; then
        run "${san_check}" asan
        assert_failure
        # TODO (https://github.com/Ruzovej/cxxet/issues/96) fix this ... there's a difference between what it reports when compiled with `gcc` and `clang` (1st has space `'int [2]'`, 2nd doesn't `'int[2]'`):
        assert_output --partial "runtime error: index 2 out of bounds for type "
        #assert_output --partial "runtime error: index 2 out of bounds for type 'int [2]'"

        run "${san_check}" lsan
        assert_failure
        assert_output --partial "==ERROR: LeakSanitizer: detected memory leaks"
        # TODO (https://github.com/Ruzovej/cxxet/issues/96) fix this ... `gcc` reports the first commented thing, `clang` sometimes the second:
        #assert_output --partial "SUMMARY: AddressSanitizer: 8 byte(s) leaked in 2 allocation(s)."
        #assert_output --partial "SUMMARY: AddressSanitizer: 4 byte(s) leaked in 1 allocation(s)."

        run "${san_check}" ubsan
        assert_failure
        assert_output --partial 'runtime error: left shift of negative value -1'
    elif [[ "${CXXET_PRESET}" =~ tsan* ]]; then
        run "${san_check}" tsan
        assert_failure
        assert_output --partial 'WARNING: ThreadSanitizer: data race'
        # TODO (https://github.com/Ruzovej/cxxet/issues/96) fix this ... `gcc` reports the first commented thing, `clang` the second:
        #assert_output --partial 'ThreadSanitizer: reported 1 warnings'
        #assert_output --partial 'ThreadSanitizer: reported 2 warnings'
        assert_output --partial 'ThreadSanitizer: reported'
    else
        skip "sanitizers not enabled in this preset (${CXXET_PRESET})"
    fi
}
