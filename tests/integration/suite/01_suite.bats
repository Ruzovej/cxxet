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

@test "Duration markers example" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_duration_1"
    local result="${CXXET_RESULTS_DIR}/example_duration.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 32

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["B","E"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["","Pyramid level","RAII duration test","RAII inner duration","RAII outer duration","RAII thread duration test","main - joining threads","main - spawning threads","manual duration test","manual inner duration","manual outer duration","manual thread duration test"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "Pyramid level")] | length' "${result}")" 6

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Complete markers example" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_complete_1"
    local result="${CXXET_RESULTS_DIR}/example_complete.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 13

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["X"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["main","pyramid","scope 1.1","scope 2.1","scope 2.2"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "pyramid")] | length' "${result}")" 9

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("dur") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Instant markers example 1" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_instant_1"
    local result="${CXXET_RESULTS_DIR}/example_instant_1.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 5

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["i"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.s) | unique | sort' "${result}")" '["t"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["main thread flushing all markers","main thread started","thread 1 started","thread 2 started","thread 3 started"]'

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("s") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Instant markers example 2" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_instant_2"
    local result="${CXXET_RESULTS_DIR}/example_instant_2.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 9

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["X","i"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result}")" 5

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result}")" 4

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["main terminating","main thread beginning","main thread, local scope","thread 1","thread 1 started","thread 2","thread 2 started","thread 3","thread 3 started"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | all(has("name") and has("ph") and has("ts") and has("s") and has("pid") and has("tid"))' "${result}")" 'true'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph != "i")] | all(has("name") and has("ph") and has("ts") and has("dur") and has("pid") and has("tid") and (has("s") | not))' "${result}")" 'true'
}

@test "Counter markers example 1" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_counter_1"
    local result="${CXXET_RESULTS_DIR}/example_counter_1.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 12

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["C"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["Counter"]'

    assert_equal "$(jq -e '.traceEvents | all(has("args"))' "${result}")" 'true'

    assert_equal "$(jq -e -c '.traceEvents | map(.args | keys[]) | unique | sort' "${result}")" '["RAM [MB]","cpu utilization","thread 1 operations","thread 2 operations"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.args."RAM [MB]")] | length' "${result}")" 5

    assert_equal "$(jq -e '[.traceEvents[] | select(.args."cpu utilization")] | length' "${result}")" 5

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("args") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Counter markers example 2" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_counter_2"
    local result="${CXXET_RESULTS_DIR}/example_counter_2.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 20003

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["C","X"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result}")" 20000

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result}")" 3

    assert_equal "$(jq -e -c '[.traceEvents[] | select(.ph == "C")] | map(.args | keys[]) | unique | sort' "${result}")" '["x","y"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["CXXET_sink_thread_reserve","Counter","Counter example 2","Euler method iterations"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | all(has("name") and has("ph") and has("ts") and has("args") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Metadata markers example" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_metadata_1"
    local result="${CXXET_RESULTS_DIR}/example_metadata_1.json"

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

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 18
    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["B","E","M","X","i"]'
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "B")] | length' "${result}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "E")] | length' "${result}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "M")] | length' "${result}")" 9
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result}")" 3
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result}")" 2

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "M")] | all(has("name") and has("ph") and has("args") and has("pid") and has("tid"))' "${result}")" 'true'

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "process_name")] | length' "${result}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "process_sort_index")] | length' "${result}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "process_labels")] | length' "${result}")" 3

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "thread_name")] | length' "${result}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "thread_sort_index")] | length' "${result}")" 2

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "process_name")] | .[0].args.name' "${result}")" "\"${executable}\""
    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "process_sort_index")] | .[0].args.sort_index' "${result}")" 5
    assert_equal "$(jq -e -c '[.traceEvents[] | select(.name == "process_labels")] | map(.args.labels) | sort' "${result}")" '["test label 3","test_label_1","test_label_2"]'

    assert_equal "$(jq -e -c '[.traceEvents[] | select(.name == "thread_name")] | map(.args.name) | sort' "${result}")" '["main thread in this example :-)","spawned thread"]'
    assert_equal "$(jq -e -c '[.traceEvents[] | select(.name == "thread_sort_index")] | map(.args.sort_index) | sort' "${result}")" '[10,50]'
}

# TODO end-user usage:
# * All event types in one file.

@test "Properly read env. variables" {
    local executable="${CXXET_BIN_DIR}/cxxet_test_reading_env"

    local result1="${CXXET_RESULTS_DIR}/example_test_reading_env_1.json"
    export CXXET_TARGET_FILENAME="${result1}"
    run "${executable}_bare"
    assert_success
    assert_output ""
    refute_sanitizer_output

    refute [ -f "${result1}" ]

    local result2="${CXXET_RESULTS_DIR}/example_test_reading_env_2.json"
    export CXXET_TARGET_FILENAME="${result2}"
    export CXXET_DEFAULT_BLOCK_SIZE=1
    run "${executable}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 1
Deduced CXXET_TARGET_FILENAME: ${result2}"
    refute_sanitizer_output

    assert [ -f "${result2}" ]
    assert_equal "$(jq -e '.traceEvents | length' "${result2}")" 3
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result2}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result2}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result2}")" 1

    local result3="${CXXET_RESULTS_DIR}/example_test_reading_env_3.json"
    export CXXET_VERBOSE=0
    export CXXET_TARGET_FILENAME="${result3}"
    run "${executable}"
    assert_success
    assert_output ""
    refute_sanitizer_output

    assert [ -f "${result3}" ]
    assert_equal "$(jq -e '.traceEvents | length' "${result3}")" 3
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result3}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result3}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result3}")" 1

    local result4="${CXXET_RESULTS_DIR}/example_test_reading_env_4.pid{pid}.json.XXXXXX"
    export CXXET_VERBOSE=1
    export CXXET_DEFAULT_BLOCK_SIZE=40
    export CXXET_TARGET_FILENAME="${result4}"
    run "${executable}"
    assert_success
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 40
Deduced CXXET_TARGET_FILENAME: ${result4}"
    assert_output --partial "Writing out events to file: ${result4%\{pid\}.json.XXXXXX}"
    refute_sanitizer_output

    result4="$(printf '%s' "${output}" | grep -oP '(?<=Writing out events to file: ).*')"
    assert [ -f "${result4}" ]
    assert_equal "$(jq -e '.traceEvents | length' "${result4}")" 3
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result4}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result4}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result4}")" 1

    unset CXXET_TARGET_FILENAME # reset to default (empty) value
    local result5='/tmp/cxxet_default.pid{pid}.json.XXXXXX'
    run "${executable}"
    assert_success
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 40
Deduced CXXET_TARGET_FILENAME: ${result5}"
    assert_output --partial "Writing out events to file: ${result5%\{pid\}.json.XXXXXX}"
    refute_sanitizer_output

    result5="$(printf '%s' "${output}" | grep -oP '(?<=Writing out events to file: ).*')"
    assert [ -f "${result5}" ]
    assert_equal "$(jq -e '.traceEvents | length' "${result5}")" 3
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result5}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result5}")" 1
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result5}")" 1
}
