#!/usr/bin/env bash

set -e

cxxet_include scripts/common/docker_run
cxxet_include scripts/common/docker_validate_image_name_base
cxxet_include scripts/common/ensure_docker_is_allowed

function docker_interactive() {
    ensure_docker_is_allowed

    function usage() {
        printf 'Usage: docker_interactive <image_name_base>\n' >&2
    }

    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        usage
        return 0
    fi

    local image_name_base="$1"

    docker_validate_image_name_base "${image_name_base}" || {
        usage
        exit 1
    }

    docker_run \
        --interactive \
        --tty \
        --hostname "${image_name_base}" \
        "$(docker_image_name "${image_name_base}")" \
        bash
}
