#!/usr/bin/env bash

set -e

function dockerfile_name() {
    local dockerfile_path="${CXXET_ROOT_DIR}/docker/${1:?}.Dockerfile"
    printf '%s\n' "${dockerfile_path}"
}
