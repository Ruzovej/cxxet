#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/adjust_cxxet_env_variables"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_CXXET_BIN_DIR"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

function setup_file() {
    adjust_cxxet_env_variables

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
