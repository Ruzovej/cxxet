#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_CXXET_BIN_DIR"
#load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

function setup_file() {
    populate_CXXET_BIN_DIR

    if [[ ! -f "${CXXET_BIN_DIR}/libcxxet.so" ]]; then
        assert [ -f "${CXXET_BIN_DIR}/libcxxet.a" ]
        skip "shared lib. not found - built as a static library"
    fi
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

@test "Shared library symbol visibility" {
    run nm -D -C "${CXXET_BIN_DIR}/libcxxet.so"
    assert_success
    # only those symbols should be exported - more detailed checks are done via `abidiff` (see `scripts/commands/abi_check.bash`):
    assert_output --partial "cxxet::"
    # those definitely not:
    refute_output --partial "doctest::"
    refute_output --partial "cxxet::impl::"
    refute_output --partial "benchmark::"
}

@test "Unit tests runner contains expected symbols" {
    run nm -C "${CXXET_BIN_DIR}/cxxet_unit_tests"
    assert_success
    # contains internal implementation symbols & `doctest` stuff:
    assert_output --partial "cxxet::impl::"
    assert_output --partial "doctest::"
}

@test "Micro benchmarks runner contains expected symbols" {
    run nm -C "${CXXET_BIN_DIR}/cxxet_benchmarks"
    assert_success
    # contains internal implementation symbols & `google benchmark` stuff:
    assert_output --partial "cxxet::impl::"
    assert_output --partial "benchmark::"
}

@test "Examples & unit test runner properly depend on shared library if built this way" {
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
