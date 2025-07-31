#!/usr/bin/env bash

cxxet_include "scripts/common/docker_run"

function docker_interactive() {
    local image_name_base="${1:-cxxet_compiler}"

    docker_run \
        --interactive \
        --tty \
        --hostname "${image_name_base}" \
        "$(docker_image_name "${image_name_base}")" \
        bash
}
