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
    #user_log "# results from this run are in '%s'\n" "${CXXET_RESULTS_DIR}"
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

@test "Categories example 1 - basic registration and event categorization" {
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

@test "Categories example 2 - registration fails with invalid name" {
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

@test "Categories example 3 - registration fails with invalid flag" {
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

@test "Categories example 4 - registration fails with duplicate flag" {
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

@test "Custom file_sink redirection example 1" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_local_file_sink_1"
    local result1="${CXXET_RESULTS_DIR}/example_local_file_sink_1_A.json"
    local result2="${CXXET_RESULTS_DIR}/example_local_file_sink_1_B.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result1}" "${result2}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result1}" ]
    assert [ -f "${result2}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result1}")" '"ns"'
    assert_equal "$(jq -e '.displayTimeUnit' "${result2}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result1}")" 4
    assert_equal "$(jq -e '.traceEvents | length' "${result2}")" 4

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result1}")" '["X","i"]'
    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result2}")" '["X","i"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result1}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result2}")" 2

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result1}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result2}")" 2

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result1}")" '["example: redirecting all events to custom file_sink ... first","example: redirecting all events to custom file_sink ... second","within two sleeps ... first","within two sleeps ... second"]'
    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result2}")" '["example: redirecting all events to default & global file_sink ... first","example: redirecting all events to default & global file_sink ... second","within two sleeps ... first","within two sleeps ... second"]'
}

@test "Custom file_sink redirection example 2" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_local_file_sink_2"
    local result1="${CXXET_RESULTS_DIR}/example_local_file_sink_2_A.json"
    local result2="${CXXET_RESULTS_DIR}/example_local_file_sink_2_B.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result1}" "${result2}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result1}" ]
    assert [ -f "${result2}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result1}")" '"ns"'
    assert_equal "$(jq -e '.displayTimeUnit' "${result2}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result1}")" 4
    assert_equal "$(jq -e '.traceEvents | length' "${result2}")" 4

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result1}")" '["X","i"]'
    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result2}")" '["X","i"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result1}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result2}")" 2

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result1}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result2}")" 2

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result1}")" '["example: redirecting all events to custom (thread safe) file_sink ... first","example: redirecting all events to custom (thread safe) file_sink ... second","within two sleeps ... first","within two sleeps ... second"]'
    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result2}")" '["example: redirecting all events to default & global file_sink ... first","example: redirecting all events to default & global file_sink ... second","within two sleeps ... first","within two sleeps ... second"]'
}

@test "Custom file_sink redirection example 3" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_local_file_sink_3"
    local result1="${CXXET_RESULTS_DIR}/example_local_file_sink_3_A.json"
    local result2="${CXXET_RESULTS_DIR}/example_local_file_sink_3_B.json"

    run "${executable}_bare"
    assert_success
    assert_output ""

    run "${executable}" "${result1}" "${result2}"
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    refute_sanitizer_output

    assert [ -f "${result1}" ]
    assert [ -f "${result2}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result1}")" '"ns"'
    assert_equal "$(jq -e '.displayTimeUnit' "${result2}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result1}")" 4
    assert_equal "$(jq -e '.traceEvents | length' "${result2}")" 4

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result1}")" '["X","i"]'
    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result2}")" '["X","i"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result1}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result2}")" 2

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result1}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result2}")" 2

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result1}")" '["example: redirecting all events to custom file_sink 1 (main thread)","example: redirecting all events to custom file_sink 1 (spawned thread)","within two sleeps 1 (main thread)","within two sleeps 1 (spawned thread)"]'
    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result2}")" '["example: redirecting all events to custom file_sink 2 (main thread)","example: redirecting all events to custom file_sink 2 (spawned thread)","within two sleeps 2 (main thread)","within two sleeps 2 (spawned thread)"]'
}

@test "Custom file_sink & cascade_sink redirection example 4" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_local_file_sink_4"
    local result="${CXXET_RESULTS_DIR}/example_local_file_sink_4.json"

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

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 16

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["X","i"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${result}")" 8

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result}")" 8

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["example: redirecting all events to custom file_sink","within two sleeps"]'
}

@test "Custom file_sink & custom writer example 5" {
    local executable="${CXXET_BIN_DIR}/cxxet_example_local_file_sink_5"

    run "${executable}_bare"
    assert_success
    refute_output --partial ": Custom writer initialized; output:"
    refute_output --partial "custom writer finished ... index "

    run "${executable}"
    assert_success
    refute_sanitizer_output
    refute_output --partial "Writing out events to file: "

    assert_equal "$(grep -c ": Custom writer initialized; output:" <<< "${output}")" 4
    assert_equal "$(grep -c "custom writer finished ... index " <<< "${output}")" 4

    local i
    for i in {0..3}; do
        assert_output --partial "index ${i}: Custom writer initialized; output:"
        assert_output --partial "custom writer finished ... index ${i}"

        local json_output="$(printf '%s' "${output}" | sed -n "/index ${i}: Custom writer initialized; output:/,/custom writer finished ... index ${i}/p" | sed '1d;$d')" # thanks Claude, I don't understand *how* this works, but it does :-)
        assert [ -n "${json_output}" ]

        assert_equal "$(jq -e '.traceEvents | length' <<< "${json_output}")" 5
        assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' <<< "${json_output}")" 1
        assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' <<< "${json_output}")" 1
        assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' <<< "${json_output}")" 3
        # probably not needed to check it in higher detail ...
    done
}

@test "Large benchmark correctness test 1 (cxxet_bench_mt_counter)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_1"
    local num_iters=50
    local num_threads=4
    local executable="${CXXET_BIN_DIR}/cxxet_bench_mt_counter"

    local args=(
        "${num_iters}"
        2 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "$(( num_iters * num_threads ))"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${output_file2}")" "$(( num_iters * num_threads ))"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_driver_statistics="$(( 2 + 4 * num_extracted_statistic_from_real_file_low ))" # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta` + `examples/for_benchmarks/postprocessing/src/result_processor.cxx` -> function `process_benchmark_thread_perfs` ... 6 items, only 4 of them have (7) detailed statistics
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_high ))"
    # first `*2`: `cxxet` + `bare`
    # observing one thing - gaps (or intervals ...) - in the results:
    # there are 4 * 49 gaps -> detailed statistics will consist of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_high ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_counter_marker_interval"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 2 (cxxet_bench_st_instant)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_2"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_instant"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low ))"
    # first `*2`: `cxxet` + `bare`
    # observing one thing - gaps (or intervals ...) - in the results:
    # there are 4 * 49 gaps -> detailed statistics will consist of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_instant_marker_interval"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 3 (cxxet_bench_st_guarded_instant)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_3"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_guarded_instant"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "$(( num_iters * 2 ))"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high * 2 ))"
    # first `*2`: `cxxet` + `bare`
    # observing three things ...
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high * 2 ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_guarding_complete_marker_duration"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_guarding_complete_marker_gap"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_guarded_instant_dist_from_complete_center"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 4 (cxxet_bench_st_complete)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_4"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_complete"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    # first `*2`: `cxxet` + `bare`
    # observing one thing - durations - in the results:
    # there are 50 durations -> detailed statistics will consist of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_complete_marker_duration"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_complete_marker_gap"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 5 (cxxet_bench_st_duration)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_5"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_duration"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "$(( num_iters * 2 ))"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "B")] | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "E")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]
    # TODO for the test cases above too!

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    # first `*2`: `cxxet` + `bare`
    # observing two things - durations and gaps - in the results:
    # there are 49 gaps and 50 durations ... -> detailed statistics for one will consist of 7 entries, for the other of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_duration_marker_gap"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_duration_marker_duration"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark result comparison test" {
    local executable="${CXXET_BIN_DIR}/cxxet_large_bench_compare"
    local fake_baseline="${CXXET_ROOT_DIR}/tests/integration/fake_data/large_bench_fake_res_1.json"
    local fake_challenger="${CXXET_ROOT_DIR}/tests/integration/fake_data/large_bench_fake_res_2.json"
    local results="${CXXET_RESULTS_DIR}/large_benchmark_compare_output.json"

    run "${executable}" --verbose --out-json "${results}" "${fake_baseline}" "${fake_challenger}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${results}" ]

    # TODO (https://github.com/Ruzovej/cxxet/issues/188): `sub_fake_common_baseline_better` and `sub_fake_common_challenger_better` shouldn't come out as `EQUIVALENT`!!!
    assert_output --partial "No challenger data for measurement fake_1 (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_1, impl=cxxet); skipping comparison"
    assert_output --partial "No baseline data for measurement fake_2 (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_2, impl=cxxet); skipping comparison"
    assert_output --partial "Challenger is most probably EQUIVALENT to baseline for measurement fake_common (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_common_baseline_better, impl=cxxet)"
    assert_output --partial "Challenger is most probably EQUIVALENT to baseline for measurement fake_common (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_common_challenger_better, impl=cxxet)"
    assert_output --partial "Challenger is most probably EQUIVALENT to baseline for measurement fake_common (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_common_eq, impl=cxxet)"

    # ditto:
    assert_output --partial "  Total measurements compared: 5"
    assert_output --partial "  Missing baseline measurements: 1 (20.000000 %)"
    assert_output --partial "  Missing challenger measurements: 1 (20.000000 %)"
    assert_output --partial "  Draws: 3 (60.000000 %)"
    assert_output --partial "  Challenger better: 0 (0.000000 %)"
    assert_output --partial "  Challenger worse: 0 (0.000000 %)"

    assert_output --partial "Saved comparison results into ${results}"
}

# TODO end-user usage:
# * All event types in one file.

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

@test "Provide custom writer to global/default file_sink" {
    local executable="${CXXET_BIN_DIR}/cxxet_custom_writer"

    run "${executable}_bare"
    assert_success
    refute_sanitizer_output

    run "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    assert_output --partial "Custom writer initialized; output:"
    refute_output --partial "Writing out events to file: "
    assert_output --partial "custom writer finished ..."

    local json_output="$(printf '%s' "${output}" | sed -n '/Custom writer initialized; output:/,/<--- custom writer finished .../p' | sed '1d;$d')" # thanks Claude, I don't understand *how* this works, but it does :-)
    assert [ -n "${json_output}" ]

    assert_equal "$(jq -e '.traceEvents | length' <<< "${json_output}")" 6
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' <<< "${json_output}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' <<< "${json_output}")" 2
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' <<< "${json_output}")" 2
    # probably not needed to check it in higher detail ...
}

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
