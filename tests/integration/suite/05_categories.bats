#!/usr/bin/env bats

# TODO rename this file

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

@test "Example 1 - basic registration and event categorization" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_categories_1"
    local result="${CXXET_RESULTS_DIR}/example_categories_1.json"

    run "${executable}_bare" "${result}"
    assert_success
    assert_output ""

    refute [ -f "${result}" ]

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 6
    assert_equal "$(jq -e '[.traceEvents[] | select(has("cat"))] | length' "${result}")" 4
    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["X"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["1 HTTP request to API","2 Query user data","3 Sync data to server","4 Cleanup temporary files","5 Uncategorized operation (default)","6 Uncategorized operation (ignoring unknown categories)"]'

    # Check the `cat` content for all categorized events
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "1 HTTP request to API")] | .[0].cat' "${result}")" '"network"'
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "2 Query user data")] | .[0].cat' "${result}")" '"database"'
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "3 Sync data to server")] | .[0].cat' "${result}")" '"network,database"'
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "4 Cleanup temporary files")] | .[0].cat' "${result}")" '"background-tasks"'

    # Check that uncategorized events don't have `cat` field
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "5 Uncategorized operation (default)")] | .[0] | has("cat")' "${result}")" 'false'
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "6 Uncategorized operation (ignoring unknown categories)")] | .[0] | has("cat")' "${result}")" 'false'
}

@test "Example 2 - registration fails with invalid name" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_fail_categories_2"
    local result="${CXXET_RESULTS_DIR}/example_fail_categories_2.json"

    run "${executable}_bare" "${result}"
    assert_success
    assert_output ""

    refute [ -f "${result}" ]

    run "${executable}" "${result}"
    assert_failure
    refute_sanitizer_output
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: 
terminate called after throwing an instance of 'std::runtime_error'
  what():  category name is not valid"

    refute [ -f "${result}" ]
}

@test "Example 3 - registration fails with invalid flag" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_fail_categories_3"
    local result="${CXXET_RESULTS_DIR}/example_fail_categories_3.json"

    run "${executable}_bare" "${result}"
    assert_success
    assert_output ""

    refute [ -f "${result}" ]

    run "${executable}" "${result}"
    assert_failure
    refute_sanitizer_output
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: 
terminate called after throwing an instance of 'std::runtime_error'
  what():  category flag must have exactly one bit set"

    refute [ -f "${result}" ]
}

@test "Example 4 - registration fails with duplicate flag" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_fail_categories_4"
    local result="${CXXET_RESULTS_DIR}/example_fail_categories_4.json"

    run "${executable}_bare" "${result}"
    assert_success
    assert_output ""

    refute [ -f "${result}" ]

    run "${executable}" "${result}"
    assert_failure
    refute_sanitizer_output
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: 
terminate called after throwing an instance of 'std::runtime_error'
  what():  category flag already registered"

    refute [ -f "${result}" ]
}
