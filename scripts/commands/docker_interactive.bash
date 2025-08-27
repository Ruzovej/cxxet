#!/usr/bin/env bash

set -e

cxxet_include scripts/common/docker_run
cxxet_include scripts/common/docker_validate_image_name_base
cxxet_include scripts/common/ensure_docker_is_allowed
cxxet_include scripts/common/list_dockerfiles
cxxet_include scripts/common/reject_further_args

function docker_interactive() {
    ensure_docker_is_allowed

    function usage() {
        if [[ "$1" != '--short' ]]; then
            printf 'docker_interactive: start container (interactive bash) of given image\n' >&2
        fi
        printf 'Usage: docker_interactive <image_name_base|-h|--help>\n' >&2
        list_dockerfiles
    }

    if [[ "$1" == "--help" || "$1" == "-h" ]]; then
        usage
        exit 0
    fi

    local image_name_base="$1"

    docker_validate_image_name_base "${image_name_base}" && shift && reject_further_args "$@" || {
        usage --short
        exit 1
    }

    docker_run \
        --interactive \
        --tty \
        --hostname "${image_name_base}" \
        "$(docker_image_name "${image_name_base}")" \
        bash
}
