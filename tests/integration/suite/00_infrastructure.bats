#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/adjust_cxxet_env_variables"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_needed_bash_variables"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

function setup_file() {
    populate_needed_bash_variables

    adjust_cxxet_env_variables
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    :
    #user_log "# results from this run are in '%s'\n" "${CXXET_RESULTS_DIR}"
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
        # well, to be precise, each consists of "undefined behavior" so it's just a lucky coincidence that it succeeds:
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

@test "Shared library symbol visibility" {
    local shared_lib="${CXXET_BIN_DIR}/libcxxet.so"

    if ! [[ -f "${shared_lib}" ]]; then
        skip "shared lib. not found - probably built as a static library"
    fi

    run nm -D -C "${shared_lib}"
    assert_success
    # only those symbols should be exported - more detailed checks are done via `abidiff` (see `scripts/commands/abi_check.bash`):
    assert_output --partial "cxxet::"
    # those definitely not:
    refute_output --partial "doctest::"
    refute_output --partial "cxxet::impl::"
}

@test "Unit tests runner contains expected symbols" {
    if ! [[ -f "${CXXET_BIN_DIR}/libcxxet.so" ]]; then
        skip "shared lib. not found - probably built as a static library"
    fi

    run nm -C "${CXXET_BIN_DIR}/cxxet_unit_tests"
    assert_success
    # contains internal implementation symbols & `doctest` stuff:
    assert_output --partial "cxxet::impl::"
    assert_output --partial "doctest::"
}

@test "Examples & unit test runner properly depend on shared library if built this way" {
    if ! [[ -f "${CXXET_BIN_DIR}/libcxxet.so" ]]; then
        skip "shared lib. not found - probably built as a static library"
    fi

    for file in "${CXXET_BIN_DIR}"/cxxet_*; do
        # user_log "# checking file '%s'\n" "${file}"
        run ldd "${file}"
        assert_success
        if [[ "${file}" =~ _bare$ \
            || "${file}" =~ cxxet_unit_tests \
            || "${file}" =~ cxxet_benchmarks \
            || "${file}" =~ cxxet_large_bench_postprocess \
            || "${file}" =~ cxxet_large_bench_compare \
        ]]; then
            refute_output --partial "libcxxet.so"
        else
            assert_output --partial "libcxxet.so"
        fi
    done
}
