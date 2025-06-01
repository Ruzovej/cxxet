#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"

function user_log() {
    local fmt_string="$1"
    shift
    printf "${fmt_string}" "$@" >&3
}

function setup_file() {
    user_log "# configuring and building with preset '%s' ... " "${RSM_PRESET}"
    ./compile.bash \
        -DRSM_BUILD_TESTS=ON \
        --preset "${RSM_PRESET}" \
        --target rsm_dummy_app \
        --target rsm_infra_sanitizer_check \
        --target rsm_examples \
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

@test "sanitizers work as expected" {
    local san_check="${BIN_DIR}/rsm_infra_sanitizer_check"
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

# TODO delete later:
@test "dummy app reports all markers with no sanitizer issues" {
    run "${BIN_DIR}/rsm_dummy_app"

    # Verify that no sanitizer errors are reported
    refute_output --partial "Sanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "runtime error"
    refute_output --partial "ERROR:"
    refute_output --partial "WARNING:"

    # Verify that all markers are reported in the output
    assert_output --partial "Deduced RSM_DEFAULT_BLOCK_SIZE: 2"
    assert_output --partial "Deduced RSM_OUTPUT_FORMAT: 0"
    assert_output --partial "Deduced RSM_TARGET_FILENAME: "
    assert_output --partial ": 'loop', color -1, tag -1: "
    assert_output --partial ": 'int store', color -1, tag -1: "
    assert_output --partial ": 'int load', color -1, tag -1: "
    assert_output --partial ": 'scoped 1', color -1, tag -1: "
    assert_output --partial ": 'scoped 2', color -1, tag -1: "
    assert_output --partial ": 'scoped 3', color -1, tag -1: "
    assert_output --partial ": 'scoped 4', color -1, tag -1: "
    assert_output --partial ": 'scoped 5 (macro with both default color and tag)', color -1, tag -1: "
    assert_output --partial ": 'scoped 6 (macro with explicit color and default tag)', color 1, tag -1: "
    assert_output --partial ": 'scoped 7 (macro with both explicit color and tag)', color 1, tag 2: "
    assert_output --partial ": 'first local macro marker', color -1, tag 0: "
    assert_output --partial ": 'second local macro marker testing no shadowing occurs', color -1, tag 0: "
    assert_output --partial ": 'third local macro marker testing no shadowing occurs', color -1, tag 0: "
    assert_output --partial ": 'first local macro marker', color -1, tag 1: "
    assert_output --partial ": 'second local macro marker testing no shadowing occurs', color -1, tag 1: "
    assert_output --partial ": 'third local macro marker testing no shadowing occurs', color -1, tag 1: "
    assert_output --partial ": 'scoped 8 (in 3 various parallel threads)', color -1, tag 0: "
    assert_output --partial ": 'scoped 8 (in 3 various parallel threads)', color -1, tag 1: "
    assert_output --partial ": 'scoped 8 (in 3 various parallel threads)', color -1, tag 2: "
    assert_output --partial ": 'fourth local macro marker testing no shadowing occurs', color -1, tag 0: "
    assert_output --partial ": 'fourth local macro marker testing no shadowing occurs', color -1, tag 1: "
    assert_output --partial ": 'fourth local macro marker testing no shadowing occurs', color -1, tag 2: "
    assert_output --partial ": 'fourth local macro marker testing no shadowing occurs', color -1, tag 3: "
    assert_output --partial ": 'fourth local macro marker testing no shadowing occurs', color -1, tag 4: "
    assert_output --partial ": 'fourth local macro marker testing no shadowing occurs', color -1, tag 5: "
    #assert_output --partial ": '', color -1, tag -1: "

    # TODO how to check that output consists of exactly above mentioned `N` lines?!

    assert_success
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

    assert [ -f "${result}" ]
}

@test "Instant markers example 1" {
    local executable="${BIN_DIR}/rsm_example_instant_1"
    local result="${TMP_RESULT_DIR}/example_instant_1.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "

    assert [ -f "${result}" ]
}

@test "Instant markers example 2" {
    local executable="${BIN_DIR}/rsm_example_instant_2"
    local result="${TMP_RESULT_DIR}/example_instant_2.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "

    assert [ -f "${result}" ]
}

@test "Counter markers example 1" {
    local executable="${BIN_DIR}/rsm_example_counter_1"
    local result="${TMP_RESULT_DIR}/example_counter_1.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "

    assert [ -f "${result}" ]
}

@test "Counter markers example 2" {
    local executable="${BIN_DIR}/rsm_example_counter_2"
    local result="${TMP_RESULT_DIR}/example_counter_2.json"

    run "${executable}" "${result}"
    assert_success
    assert_output "Deduced RSM_OUTPUT_FORMAT: 0
Deduced RSM_DEFAULT_BLOCK_SIZE: 2
Deduced RSM_TARGET_FILENAME: "

    assert [ -f "${result}" ]
}
