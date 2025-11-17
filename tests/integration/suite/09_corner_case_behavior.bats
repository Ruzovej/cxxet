#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/adjust_cxxet_env_variables"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_CXXET_BIN_DIR"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/prepare_CXXET_RESULTS_DIR"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

function setup_file() {
    adjust_cxxet_env_variables

    populate_CXXET_BIN_DIR

    prepare_CXXET_RESULTS_DIR
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

@test "Initialization alone" {
    local executable="${CXXET_BIN_DIR}/cxxet_test_init"
    local result="${CXXET_RESULTS_DIR}/example_test_init.json"

    run "${executable}_bare" "${result}"
    assert_success
    assert_output ""
    refute_sanitizer_output

    refute [ -f "${result}" ]

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    refute [ -f "${result}" ]

    export CXXET_VERBOSE=0

    run "${executable}" "${result}"
    assert_success
    assert_output ""
    refute_sanitizer_output

    refute [ -f "${result}" ]
}

@test "Empty file - forgetting to specify it" {
    if [[ "${CXXET_PRESET}" =~ .san* ]]; then
        skip "strace doesn't work with sanitizers"
    fi

    local executable="${CXXET_BIN_DIR}/cxxet_test_empty_file"

    # bare

    local strace_output_file1="${CXXET_RESULTS_DIR}/cxxet_test_empty_file.strace.1"
    run strace -o "${strace_output_file1}" "${executable}_bare"
    assert_success
    refute_sanitizer_output
    refute_output

    assert [ -f "${strace_output_file1}" ]
    assert_equal "$(grep -c 'write(1,' "${strace_output_file1}")" 0

    # discarding all events

    local strace_output_file2="${CXXET_RESULTS_DIR}/cxxet_test_empty_file.strace.2"
    run strace -o "${strace_output_file2}" "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "

    assert [ -f "${strace_output_file2}" ]
    # TODO why writing to `std::cout` (as asserted above) doesn't show up here?
    #assert_equal "$(grep -c 'write(1,' "${strace_output_file2}")" 1 # writes to `stdout`
    assert_equal "$(grep -c 'write([^1]' "${strace_output_file2}")" 0 # no other writes

    # specifying file, but no events recorded -> no file created

    local strace_output_file3="${CXXET_RESULTS_DIR}/cxxet_test_empty_file.strace.3"
    local output_file="${CXXET_RESULTS_DIR}/cxxet_test_empty_file.json"
    export CXXET_TARGET_FILENAME="${output_file}"
    run strace -o "${strace_output_file3}" "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: ${output_file}"
    refute [ -f "${output_file}" ]

    assert [ -f "${strace_output_file3}" ]
    # TODO why writing to `std::cout` (as asserted above) doesn't show up here?
    #assert_equal "$(grep -c 'write(1,' "${strace_output_file3}")" 1 # ditto
    assert_equal "$(grep -c 'write([^1]' "${strace_output_file3}")" 0 # ditto
}

@test "Implicit file - default and modified behavior" {
    local executable="${CXXET_BIN_DIR}/cxxet_test_empty_file"

    unset CXXET_TARGET_FILENAME # reset to default (empty) value

    run "${executable}_bare"
    assert_success
    refute_sanitizer_output
    assert_output ''

    run "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: /tmp/cxxet_default.pid{pid}.json.XXXXXX"

    local output_file="${CXXET_RESULTS_DIR}/cxxet_test_empty_file.pid{pid}.json.XXXXXX"
    export CXXET_TARGET_FILENAME="${output_file}"
    run "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: ${output_file}"
    refute [ -f "${output_file}" ]
}

@test "Split recorded events into multiple files" {
    local executable="${CXXET_BIN_DIR}/cxxet_test_split_files"
    local result1="${CXXET_RESULTS_DIR}/example_test_split_1.json"
    local result2="${CXXET_RESULTS_DIR}/example_test_split_2.json"
    local result3="${CXXET_RESULTS_DIR}/example_test_split_3.json"

    run "${executable}_bare" "${result1}" "${result2}" "${result3}"
    assert_success
    assert_output ""
    refute_sanitizer_output

    refute [ -f "${result1}" ]
    refute [ -f "${result2}" ]
    refute [ -f "${result3}" ]

    run "${executable}" "${result1}" "${result2}" "${result3}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result1}" ] # should contain exactly one `complete`:
    assert_equal "$(jq -e '.traceEvents | length' "${result1}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result1}")" 1

    assert [ -f "${result2}" ] # should contain exactly one `instant`:
    assert_equal "$(jq -e '.traceEvents | length' "${result2}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result2}")" 1

    assert [ -f "${result3}" ] # should contain exactly one `counter`:
    assert_equal "$(jq -e '.traceEvents | length' "${result3}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result3}")" 1
}

@test "Suboptimal initialization 1" {
    local executable="${CXXET_BIN_DIR}/cxxet_test_suboptimal_init_1"

    run "${executable}" 
    refute_sanitizer_output
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
}

@test "Suboptimal initialization 2" {
    local executable="${CXXET_BIN_DIR}/cxxet_test_suboptimal_init_2"

    run "${executable}" # no output file -> writes to `stdout`
    refute_sanitizer_output
    assert_success
    assert_output --partial 'Deduced CXXET_DEFAULT_BLOCK_SIZE: 2'
    assert_output --partial '"name":"Suboptimal duration begin","ph":"B"'
    assert_output --partial '"name":"Some complete","ph":"X"'
    assert_output --partial '"name":"Some duration end","ph":"E"'
}
