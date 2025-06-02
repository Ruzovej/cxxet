#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"

function user_log() {
    local fmt_string="$1"
    shift
    printf "${fmt_string}" "$@" >&3
}

function setup_file() {
    run which jq
    assert_success

    user_log "# configuring and building with preset '%s' ... " "${RSM_PRESET}"
    ./compile.bash \
        -DRSM_BUILD_TESTS=ON \
        -DRSM_BUILD_EXAMPLES=ON \
        --preset "${RSM_PRESET}" \
        --target infra_sanitizer_check \
        --target rsm_examples \
        --target rsm_unit_tests \
        --polite-ln-compile_commands # 2>&3 1>&3 # TODO use or delete? This displays the output of it in console ...
    user_log 'done\n'

    export BIN_DIR="bin/${RSM_PRESET}"
    export RSM_DEFAULT_BLOCK_SIZE=2
    export RSM_VERBOSE=1
    export TMP_RESULT_DIR="$(mktemp -d "${TMPDIR:-/tmp}/rsm.suite.bats.${RSM_PRESET}.XXXXXX")"
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    rm -rf "${TMP_RESULT_DIR}"
    #user_log "# results from this run are in '%s'\n" "${TMP_RESULT_DIR}"
}

# TODO later remove those ...
#
#function some_function() {
#    local msg="${1:-default message}"
#    local ret_code="${2:-0}"
#
#    printf '%s\n' "${msg}"
#    return "${ret_code}"
#}
#
#@test "first" {
#    user_log '# doing %s stuff ...\n' '1st'
#    run some_function 'doing 1st stuff ...'
#    assert_success
#    assert_output --partial '1st'
#}
#
#@test "second" {
#    user_log '# doing 2nd stuff ...\n'
#    run some_function '2nd' 13
#    assert_failure 13
#    assert_output '2nd'
#}

@test "Sanitizers work as expected" {
    local san_check="${BIN_DIR}/infra_sanitizer_check"
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
    local executable="${BIN_DIR}/rsm_example_duration_1"
    local result="${TMP_RESULT_DIR}/example_duration.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 32

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["B","E"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["","Pyramid level","RAII duration test","RAII inner duration","RAII outer duration","RAII thread duration test","main - joining threads","main - spawning threads","manual duration test","manual inner duration","manual outer duration","manual thread duration test"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "Pyramid level")] | length' "${result}")" 6

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Complete markers example" {
    local executable="${BIN_DIR}/rsm_example_complete_1"
    local result="${TMP_RESULT_DIR}/example_complete.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 13

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["X"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["main","pyramid","scope 1.1","scope 2.1","scope 2.2"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.name == "pyramid")] | length' "${result}")" 9

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("dur") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Instant markers example 1" {
    local executable="${BIN_DIR}/rsm_example_instant_1"
    local result="${TMP_RESULT_DIR}/example_instant_1.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 5

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["i"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.s) | unique | sort' "${result}")" '["t"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["main thread flushing all markers","main thread started","thread 1 started","thread 2 started","thread 3 started"]'

    assert_equal "$(jq -e '.traceEvents | all(has("name") and has("ph") and has("ts") and has("s") and has("pid") and has("tid"))' "${result}")" 'true'
}

@test "Instant markers example 2" {
    local executable="${BIN_DIR}/rsm_example_instant_2"
    local result="${TMP_RESULT_DIR}/example_instant_2.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"

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
    local executable="${BIN_DIR}/rsm_example_counter_1"
    local result="${TMP_RESULT_DIR}/example_counter_1.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"

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
    local executable="${BIN_DIR}/rsm_example_counter_2"
    local result="${TMP_RESULT_DIR}/example_counter_2.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"

    assert [ -f "${result}" ]

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 20003

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["C","X"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${result}")" 20000

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${result}")" 3

    assert_equal "$(jq -e -c '[.traceEvents[] | select(.ph == "C")] | map(.args | keys[]) | unique | sort' "${result}")" '["x","y"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["Counter","Counter example 2","Euler method iterations","RSM_thread_local_sink_reserve"]'

    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | all(has("name") and has("ph") and has("ts") and has("args") and has("pid") and has("tid"))' "${result}")" 'true'
}

# TODO end-user usage:
# * Empty file (e.g., due to no trace events in the source code, not taking the branch where they are, or forgetting to manually flush it).
# * No file at all (e.g., not specifying it in the source code, or overwriting it there when taken from an environment variable).
# * Test that all related environment variables are correctly obtained and printed out.
# * Manual dumping (without `defer = true`) into multiple files from a single process.
# * All event types in one file.

@test "Shared library symbol visibility" {
    local shared_lib="${BIN_DIR}/librsm.so"

    if ! [[ -f "${shared_lib}" ]]; then
        return
    fi

    run nm -D -C "${shared_lib}"
    assert_success
    local nm_output="${output}"

    # only those symbols should be exported:
    assert_equal "$(printf '%s' "${nm_output}" | grep -c " RSM_")" 5
    assert_equal "$(printf '%s' "${nm_output}" | grep -c " rsm::")" 1

    # those definitely not:
    assert_equal "$(printf '%s' "${nm_output}" | grep -c " doctest::")" 0
    assert_equal "$(printf '%s' "${nm_output}" | grep -c " rsm::impl::")" 0
}

@test "Unit tests runner contains expected symbols" {
    if ! [[ -f "${BIN_DIR}/librsm.so" ]]; then
        return
    fi

    run nm -C "${BIN_DIR}/rsm_unit_tests"
    assert_success
    local nm_output="$output"

    # contains internal implementation symbols & `doctest` stuff:
    assert_not_equal "$(printf '%s' "${nm_output}" | grep -c " rsm::")" 0
    assert_not_equal "$(printf '%s' "${nm_output}" | grep -c " doctest::")" 0
}

@test "Examples & unit test runner properly depend on shared library if built this way" {
    if ! [[ -f "${BIN_DIR}/librsm.so" ]]; then
        return
    fi

    run ldd "${BIN_DIR}/rsm_unit_tests"
    refute_output --partial "librsm.so"

    run ldd "${BIN_DIR}/rsm_example_complete_1"
    assert_output --partial "librsm.so"

    run ldd "${BIN_DIR}/rsm_example_counter_2"
    assert_output --partial "librsm.so"

    run ldd "${BIN_DIR}/rsm_example_instant_1"
    assert_output --partial "librsm.so"

    run ldd "${BIN_DIR}/rsm_example_counter_1"
    assert_output --partial "librsm.so"

    run ldd "${BIN_DIR}/rsm_example_duration_1"
    assert_output --partial "librsm.so"

    run ldd "${BIN_DIR}/rsm_example_instant_2"
    assert_output --partial "librsm.so"
}
