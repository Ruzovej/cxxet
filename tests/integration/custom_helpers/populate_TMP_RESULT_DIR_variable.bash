#!/usr/bin/env bash

function populate_TMP_RESULT_DIR_variable() {
    local tests_filepath="${1:?No file specified!}"
    local tests_filename="$(filename "${tests_filepath}")"
    local tests_name="${tests_filename%.bats}"

    export TMP_RESULT_DIR="${TMP_RESULT_DIR_BASE}/${CXXET_PRESET}/${tests_name:?No test name derived!}"
    mkdir -p "${TMP_RESULT_DIR}"

    user_log "# using tmp dir '%s'\n" "${TMP_RESULT_DIR}"
}
