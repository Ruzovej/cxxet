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

COMMAND_NAME="$1"
shift
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
    printf 'Available commands:\n' >&2
    for POSSIBLE_COMMAND in "${CXXET_ROOT_DIR}/scripts/commands/"*.bash; do
        printf ' - %s\n' "$(basename "${POSSIBLE_COMMAND}" .bash)" >&2
    done
    exit "${RET_CODE}"
fi

function cxxet_include() {
    local module_name="${1:?}"
    local module_path="${CXXET_ROOT_DIR:?}/${module_name}.bash"

    if [[ ! -f "${module_path}" ]]; then
        printf 'Module "%s" not found!\n' "${module_name}" >&2
        exit 1
    fi

    local loaded
    for loaded in "${CXXET_LOADED_MODULES[@]}"; do
        if [[ "${loaded}" == "${module_name}" ]]; then
            return 0
        fi
    done
    CXXET_LOADED_MODULES+=("${module_name}")
    export CXXET_LOADED_MODULES
    source "${module_path}"
}
export -f cxxet_include

bash -c "cxxet_include scripts/commands/${COMMAND_NAME} && ${COMMAND_NAME} $*"
