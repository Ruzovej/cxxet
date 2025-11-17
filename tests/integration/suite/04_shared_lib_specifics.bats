#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_CXXET_BIN_DIR"
#load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

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
