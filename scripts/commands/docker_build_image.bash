#!/usr/bin/env bash

cxxet_include "scripts/common/docker_image_name"

function docker_build_image() {
    local user_id="$(id -u)"
    local group_id="$(id -g)"
    local user_name="$(id -un)"
    local group_name="$(id -gn)"

    local image_name_base="${1:-cxxet_compiler}"

    local tag="$(docker_image_name "${image_name_base}")"

    (
        set -x
        docker build \
            --build-arg USER_ID="${user_id}" \
            --build-arg GROUP_ID="${group_id}" \
            --build-arg USER_NAME="${user_name}" \
            --build-arg GROUP_NAME="${group_name}" \
            --file "${CXXET_ROOT_DIR}/docker/${image_name_base}.Dockerfile" \
            --tag "${tag:?}" \
            "${CXXET_ROOT_DIR}/docker"
    )
}
