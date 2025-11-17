#!/usr/bin/env bash

function prepare_CXXET_RESULTS_DIR() {
    local tests_filepath="${BATS_TEST_FILENAME:?}"
    local tests_filename="$(filename "${tests_filepath}")"
    local tests_name="${tests_filename%.bats}"

    export CXXET_RESULTS_DIR="${TMP_RESULT_DIR_BASE}/${CXXET_PRESET}/${tests_name:?No test name derived!}"
    mkdir -p "${CXXET_RESULTS_DIR}"

    user_log "# using dir '%s' for results\n" "${CXXET_RESULTS_DIR}"
}
