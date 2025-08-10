#!/usr/bin/env bash

set -e

cxxet_include scripts/common/docker_image_name
cxxet_include scripts/common/docker_validate_image_name_base
cxxet_include scripts/common/dockerfile_name
cxxet_include scripts/common/ensure_docker_is_allowed
cxxet_include scripts/common/list_dockerfiles
cxxet_include scripts/common/reject_further_args

function docker_build_image() {
    ensure_docker_is_allowed

    function usage() {
        if [[ "$1" != '--short' ]]; then
            printf 'docker_build_image: builds given docker image\n' >&2
        fi
        printf 'Usage: docker_build_image <image_name_base>\n' >&2
        list_dockerfiles
    }

    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        usage
        exit 0
    fi

    local image_name_base="$1"
    shift

    docker_validate_image_name_base "${image_name_base}" && reject_further_args "$@" || {
        usage --short
        exit 1
    }

    local user_id="$(id -u)"
    local group_id="$(id -g)"
    local user_name="$(id -un)"
    local group_name="$(id -gn)"

    local dockerfile_path="$(dockerfile_name "${image_name_base}")"
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
