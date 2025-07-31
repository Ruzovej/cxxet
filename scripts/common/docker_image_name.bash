#!/usr/bin/env bash

set -e

function docker_image_name() {
    if [[ -z "${CXXET_AVOID_DOCKER}" ]]; then
        if ! command -v docker &> /dev/null; then
            printf 'Error: Docker is not installed or not in PATH\n' >&2
            exit 1
        fi

        printf '%s\n' "${1:?}:${2:-latest}"
    fi
}
