#!/usr/bin/env bats

# TODO rename this file

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output"

function setup_file() {
    export BIN_DIR="${CXXET_ROOT_DIR}/bin/${CXXET_PRESET}"
    export CXXET_VERBOSE=1
    export CXXET_DEFAULT_BLOCK_SIZE=2
    export CXXET_TARGET_FILENAME='' # by default disable dumping events into "implicit" file
    export TMP_RESULT_DIR="${TMP_RESULT_DIR_BASE}/${CXXET_PRESET}/01_suite"
    mkdir -p "${TMP_RESULT_DIR}"

    user_log "# using tmp dir '%s'\n" "${TMP_RESULT_DIR}"
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    :
    #user_log "# results from this run are in '%s'\n" "${TMP_RESULT_DIR}"
}

# TODO move out this test case to its own `*.bats` file & extract the binary from `CMakeLists.txt` to it's own, standalone one
@test "Sanitizers work as expected" {
    local san_check="${BIN_DIR}/infra_sanitizer_check"
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

@test "Duration markers example" {
    local executable="${BIN_DIR}/cxxet_example_duration_1"
    local result="${TMP_RESULT_DIR}/example_duration.json"

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
    local executable="${BIN_DIR}/cxxet_example_complete_1"
    local result="${TMP_RESULT_DIR}/example_complete.json"

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
    local executable="${BIN_DIR}/cxxet_example_instant_1"
    local result="${TMP_RESULT_DIR}/example_instant_1.json"

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
    local executable="${BIN_DIR}/cxxet_example_instant_2"
    local result="${TMP_RESULT_DIR}/example_instant_2.json"

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
    local executable="${BIN_DIR}/cxxet_example_counter_1"
    local result="${TMP_RESULT_DIR}/example_counter_1.json"

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
    local executable="${BIN_DIR}/cxxet_example_counter_2"
    local result="${TMP_RESULT_DIR}/example_counter_2.json"

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

@test "Custom file_sink redirection example 1" {
    local executable="${BIN_DIR}/cxxet_example_local_file_sink_1"
    local result1="${TMP_RESULT_DIR}/example_local_file_sink_1_A.json"
    local result2="${TMP_RESULT_DIR}/example_local_file_sink_1_B.json"

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
    local executable="${BIN_DIR}/cxxet_example_local_file_sink_2"
    local result1="${TMP_RESULT_DIR}/example_local_file_sink_2_A.json"
    local result2="${TMP_RESULT_DIR}/example_local_file_sink_2_B.json"

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
    local executable="${BIN_DIR}/cxxet_example_local_file_sink_3"
    local result1="${TMP_RESULT_DIR}/example_local_file_sink_3_A.json"
    local result2="${TMP_RESULT_DIR}/example_local_file_sink_3_B.json"

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
    local executable="${BIN_DIR}/cxxet_example_local_file_sink_4"
    local result="${TMP_RESULT_DIR}/example_local_file_sink_4.json"

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
    local executable="${BIN_DIR}/cxxet_example_local_file_sink_5"

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

# TODO end-user usage:
# * All event types in one file.

@test "Initialization alone" {
    local executable="${BIN_DIR}/cxxet_test_init"
    local result="${TMP_RESULT_DIR}/example_test_init.json"

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

    local executable="${BIN_DIR}/cxxet_test_empty_file"

    run strace "${executable}_bare"
    assert_success
    refute_sanitizer_output
    refute_output --partial "write("

    run strace "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    assert_output --partial "write(1, "
    refute_output --regexp "write\([^1]" # `stdout` ... see the asserts above which requires exactly that

    local output_file="${TMP_RESULT_DIR}/cxxet_test_empty_file.json"
    export CXXET_TARGET_FILENAME="${output_file}"
    run strace "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: ${output_file}"
    assert_output --partial "write(1, "
    refute_output --regexp "write\([^1]" # ditto
    refute [ -f "${output_file}" ]       # internally this setting was overwritten ...
}

@test "Implicit file - default and modified behavior" {
    local executable="${BIN_DIR}/cxxet_test_empty_file"

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

    local output_file="${TMP_RESULT_DIR}/cxxet_test_empty_file.pid{pid}.json.XXXXXX"
    export CXXET_TARGET_FILENAME="${output_file}"
    run "${executable}"
    assert_success
    refute_sanitizer_output
    assert_output --partial "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: ${output_file}"
    refute [ -f "${output_file}" ]
}

@test "Split recorded events into multiple files" {
    local executable="${BIN_DIR}/cxxet_test_split_files"
    local result1="${TMP_RESULT_DIR}/example_test_split_1.json"
    local result2="${TMP_RESULT_DIR}/example_test_split_2.json"
    local result3="${TMP_RESULT_DIR}/example_test_split_3.json"

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
    local executable="${BIN_DIR}/cxxet_custom_writer"

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
    local executable="${BIN_DIR}/cxxet_test_reading_env"

    local result1="${TMP_RESULT_DIR}/example_test_reading_env_1.json"
    export CXXET_TARGET_FILENAME="${result1}"
    run "${executable}_bare"
    assert_success
    assert_output ""
    refute_sanitizer_output

    refute [ -f "${result1}" ]

    local result2="${TMP_RESULT_DIR}/example_test_reading_env_2.json"
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

    local result3="${TMP_RESULT_DIR}/example_test_reading_env_3.json"
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

    local result4="${TMP_RESULT_DIR}/example_test_reading_env_4.pid{pid}.json.XXXXXX"
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
    local executable="${BIN_DIR}/cxxet_test_suboptimal_init_1"

    run "${executable}" 
    refute_sanitizer_output
    assert_success
    assert_output "Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
}

@test "Suboptimal initialization 2" {
    local executable="${BIN_DIR}/cxxet_test_suboptimal_init_2"

    run "${executable}" # no output file -> writes to `stdout`
    refute_sanitizer_output
    assert_success
    assert_output --partial 'Deduced CXXET_DEFAULT_BLOCK_SIZE: 2'
    assert_output --partial '"name":"Suboptimal duration begin","ph":"B"'
    assert_output --partial '"name":"Some complete","ph":"X"'
    assert_output --partial '"name":"Some duration end","ph":"E"'
}

@test "Shared library symbol visibility" {
    local shared_lib="${BIN_DIR}/libcxxet.so"

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
    if ! [[ -f "${BIN_DIR}/libcxxet.so" ]]; then
        skip "shared lib. not found - probably built as a static library"
    fi

    run nm -C "${BIN_DIR}/cxxet_unit_tests"
    assert_success
    # contains internal implementation symbols & `doctest` stuff:
    assert_output --partial "cxxet::impl::"
    assert_output --partial "doctest::"
}

@test "Examples & unit test runner properly depend on shared library if built this way" {
    if ! [[ -f "${BIN_DIR}/libcxxet.so" ]]; then
        skip "shared lib. not found - probably built as a static library"
    fi

    for file in "${BIN_DIR}"/cxxet_*; do
        # user_log "# checking file '%s'\n" "${file}"
        run ldd "${file}"
        assert_success
        if [[ "${file}" =~ _bare$ || "${file}" =~ cxxet_unit_tests$ ]]; then
            refute_output --partial "libcxxet.so"
        else
            assert_output --partial "libcxxet.so"
        fi
    done
}
