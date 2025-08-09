#!/usr/bin/env bash

set -e

function docker_image_name() {
    printf '%s\n' "${1:?}:${2:-latest}"
}
