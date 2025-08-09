#!/usr/bin/env bash

set -e

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
