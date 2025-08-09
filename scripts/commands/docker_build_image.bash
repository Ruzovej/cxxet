#!/usr/bin/env bash

cxxet_include scripts/common/docker_image_name

function docker_build_image() {
    function usage() {
        printf 'Usage: docker_build_image <image_name_base>\n' >&2
    }

    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        usage
        return 0
    fi

    local user_id="$(id -u)"
    local group_id="$(id -g)"
    local user_name="$(id -un)"
    local group_name="$(id -gn)"

    local image_name_base="$1"
    if [[ -z "${image_name_base}" ]]; then
        printf 'Error: Image name base is required!\n' >&2
        usage
        exit 1
    fi

    local dockerfile_path="${CXXET_ROOT_DIR}/docker/${image_name_base}.Dockerfile"
    if [[ ! -f "${dockerfile_path}" ]]; then
        printf 'Error: Dockerfile "%s" not found! Possible options are:\n' "${dockerfile_path}" >&2
        local candidate_file
        for candidate_file in "${CXXET_ROOT_DIR}/docker/"*.Dockerfile; do
            printf ' - %s\n' "$(basename "${candidate_file}" .Dockerfile)" >&2
        done
        exit 1
    fi

    local tag="$(docker_image_name "${image_name_base}")"

    (
        set -x
        docker build \
            --build-arg USER_ID="${user_id:?}" \
            --build-arg GROUP_ID="${group_id:?}" \
            --build-arg USER_NAME="${user_name:?}" \
            --build-arg GROUP_NAME="${group_name:?}" \
            --file "${dockerfile_path:?}" \
            --tag "${tag:?}" \
            "${CXXET_ROOT_DIR}/docker"
    )
}
