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

COMMAND_NAME="${1:?}"
shift

bash -c "cxxet_include scripts/commands/${COMMAND_NAME} && ${COMMAND_NAME} $*"
