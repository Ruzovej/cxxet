#!/usr/bin/env bash

set -e

function ensure_docker_is_allowed() {
    if [[ -z "${CXXET_AVOID_DOCKER}" ]]; then
        if ! command -v docker &> /dev/null; then
            printf 'Error: Docker is not installed or not in PATH\n' >&2
            exit 1
        fi

        return 0
    else
        printf 'Error: Docker is disabled by CXXET_AVOID_DOCKER environment variable\n' >&2
        exit 1
    fi
}
