#!/usr/bin/env bash

set -e

{
    CXXET_ROOT_DIR="${PWD}"

    while [[ "${CXXET_ROOT_DIR}" != '/' && ! -f "${CXXET_ROOT_DIR}/cxxet_manage.bash" ]]; do
        CXXET_ROOT_DIR="${CXXET_ROOT_DIR%/*}"
    done

    if [[ "${CXXET_ROOT_DIR}" == '/' ]]; then
        printf 'Failed to determine cxxet root directory.\n' >&2
        exit 1
    fi

    export CXXET_ROOT_DIR
}

source "${CXXET_ROOT_DIR}/scripts/base/cxxet_include.bash"
cxxet_include scripts/base/cxxet_list_files
cxxet_include scripts/base/cxxet_require_command

COMMAND_NAME="$1"
COMMAND_FILE="${CXXET_ROOT_DIR}/scripts/commands/${COMMAND_NAME}.bash"
if [[ "${COMMAND_NAME}" == "--help" || "${COMMAND_NAME}" == "-h" || -z "${COMMAND_NAME}" || ! -f "${COMMAND_FILE}" ]]; then
    RET_CODE=0
    if [[ -z "${COMMAND_NAME}" ]]; then
        printf 'Error: Command name is required!\n' >&2
        RET_CODE=1
    elif [[ "${COMMAND_NAME}" != "--help" && "${COMMAND_NAME}" != "-h" ]]; then
        printf 'Error: Command "%s" not found!\n' "${COMMAND_NAME}" >&2
        RET_CODE=1
    fi
    printf 'Usage: cxxet_manage <command> [command_specific_args...]\n' >&2
    cxxet_list_files "${CXXET_ROOT_DIR}/scripts/commands" bash
    exit "${RET_CODE}"
fi

shift

bash -c "cxxet_include scripts/commands/${COMMAND_NAME} && ${COMMAND_NAME} $*"
