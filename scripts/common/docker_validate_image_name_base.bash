#!/usr/bin/env bash

set -e

cxxet_include scripts/common/docker_image_name
cxxet_include scripts/common/dockerfile_name

function docker_validate_image_name_base() {
    local image_name_base="$1"

    if [[ -z "${image_name_base}" || ! -f "$(dockerfile_name "${image_name_base}")" ]]; then
        if [[ -z "${image_name_base}" ]]; then
            printf 'Error: Image name base is required!\n' >&2
        else
            printf 'Error: Dockerfile for image "%s" not found! Possible options are:\n' "${image_name_base}" >&2
        fi
        local candidate_file
        for candidate_file in "${CXXET_ROOT_DIR}/docker/"*.Dockerfile; do
            printf ' - %s\n' "$(basename "${candidate_file}" .Dockerfile)" >&2
        done
        return 1
    fi
    return 0
}
