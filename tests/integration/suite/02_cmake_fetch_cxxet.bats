#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

function setup_file() {
    if [[ "${CXXET_PRESET:-release}" != 'release' ]]; then
        skip "this should test only 'release' build(s), current preset is '${CXXET_PRESET}'"
    fi

    export CXXET_DEFAULT_BLOCK_SIZE=2 # torture it little bit
    export CXXET_VERBOSE=1
    export TMP_RESULT_DIR="$(mktemp -d "${TMPDIR:-/tmp}/cxxet.02_cmake_fetch_cxxet.bats.XXXXXX")"

    user_log "# using tmp dir '%s', repository in '%s' and testing its commit '%s'%s\n" \
        "${TMP_RESULT_DIR}" \
        "${CXXET_PWD:?}" \
        "${CXXET_CURRENT_COMMIT_HASH:?}" \
        "${CXXET_UNCOMMITED_CHANGES:+", !!! BUT BEWARE, THERE ARE UNCOMMITTED CHANGES THAT WON'T BE 'FETCH_CONTENT-ED' !!!"}"

    export CXXET_BUILD_DIR="${TMP_RESULT_DIR}/build"

    # When using `Ninja`, `compile_commands.json` contains a bit different paths -> default generator is fixed
    run cmake \
        -S "${CXXET_PWD}/examples/cmake_fetch_content" \
        -B "${CXXET_BUILD_DIR}" \
        -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCXXET_ROOT_DIR="${CXXET_PWD}" \
        -DCXXET_TAG="${CXXET_CURRENT_COMMIT_HASH}"
    assert_success
    #user_log '%s\n' "${output}"

    run cmake \
        --build "${CXXET_BUILD_DIR}" \
        -j "$(nproc)"
    assert_success
    #user_log '%s\n' "${output}"

    export CXXET_EXAMPLE_EXECUTABLE="${CXXET_BUILD_DIR}/cxxet_fetch_content_example"
    export CXXET_EXAMPLE_EXECUTABLE_BARE="${CXXET_BUILD_DIR}/cxxet_fetch_content_example_bare"
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    if [[ -n "${TMP_RESULT_DIR}" ]]; then
        rm -rf "${TMP_RESULT_DIR}"
        user_log '# used tmp dir erased\n'
    fi
    #user_log "# results from this run are in '%s'\n" "${TMP_RESULT_DIR}"
}

@test "Check build properties described in generated 'compile_commands.json'" {
    local compile_commands="${CXXET_BUILD_DIR}/compile_commands.json"
    assert [ -f "${compile_commands}" ]

    local cxxet_lib_source_files=("$(find "${CXXET_BUILD_DIR}/_deps/cxxet-src/src/" -type f -name '*.cxx')")
    local num_cxxet_lib_source_files="$(printf '%s\n' "${cxxet_lib_source_files[@]}" | wc -l)"
    local num_examples_built=2 # regular & bare version
    local expected_num_source_files="$((num_cxxet_lib_source_files + num_examples_built))"

    # all translation units:
    assert_equal "$(jq -e 'length' "${compile_commands}")" "${expected_num_source_files}"

    # "custom" executables source files:
    assert_equal "$(jq -e "[ .[] | select(.directory == \"${CXXET_BUILD_DIR}\") ] | length" "${compile_commands}")" "${num_examples_built}"

    assert_equal "$(jq -e -c "[ .[] | select(.directory == \"${CXXET_BUILD_DIR}\") ] | map(.file) | unique | sort" "${compile_commands}")" "[\"${CXXET_PWD}/examples/cmake_fetch_content/1.cxx\"]"

    # fetched `cxxet` source files:
    assert_equal "$(jq -e "[ .[] | select(.directory == \"${CXXET_BUILD_DIR}/_deps/cxxet-build\") ] | length" "${num_cxxet_lib_source_files}")"

    assert_equal "$(jq -e "[ .[] | select(.directory == \"${CXXET_BUILD_DIR}/_deps/cxxet-build\") ] | map(.file) | unique | sort" "${compile_commands}" | grep -c "${CXXET_BUILD_DIR}/_deps/cxxet-src/src")" "${num_cxxet_lib_source_files}"
}

@test "Executable with tracing contains expected cxxet symbols" {
    assert [ -f "${CXXET_EXAMPLE_EXECUTABLE}" ]

    run nm -C "${CXXET_EXAMPLE_EXECUTABLE}"
    assert_success
    local nm_output="${output}"

    assert_not_equal "$(printf '%s' "${nm_output}" | grep -c "cxxet")" 0
}

@test "Executable with disabled tracing doesn't contain any cxxet symbols" {
    assert [ -f "${CXXET_EXAMPLE_EXECUTABLE_BARE}" ]

    run nm -C "${CXXET_EXAMPLE_EXECUTABLE_BARE}"
    assert_success
    local nm_output="${output}"

    assert_equal "$(printf '%s' "${nm_output}" | grep -c "cxxet")" 0
}

@test "Using 'fetch_content-ed' cxxet to trace given application" {
    local result="${TMP_RESULT_DIR}/traced.json"

    run "${CXXET_EXAMPLE_EXECUTABLE}" "${result}"
    assert_success
    assert_output "Deduced CXXET_OUTPUT_FORMAT: 0
Deduced CXXET_DEFAULT_BLOCK_SIZE: 2
Deduced CXXET_TARGET_FILENAME: "
    assert [ -f "${result}" ]

    # observe only basic properties, more detailed tests are in `01_suite.bats`:

    assert_equal "$(jq -e '.displayTimeUnit' "${result}")" '"ns"'

    assert_equal "$(jq -e '.traceEvents | length' "${result}")" 11

    assert_equal "$(jq -e -c '.traceEvents | map(.ph) | unique | sort' "${result}")" '["X"]'

    assert_equal "$(jq -e -c '.traceEvents | map(.name) | unique | sort' "${result}")" '["main","other thread ...","pyramid"]'
}

@test "Using 'fetch_content-ed' cxxet_bare to not trace given application" {
    local target_file="${TMP_RESULT_DIR}/bare.json"

    run "${CXXET_EXAMPLE_EXECUTABLE_BARE}" "${target_file}"
    assert_success
    assert_output ""
    refute [ -f "${target_file}" ]
}

# TODO tests:
#   * shared library depending on `cxxet`
#       * application using it
#   * application/... using `cxxet` built as a shared library
#   * verify no examples/tests/... are built
