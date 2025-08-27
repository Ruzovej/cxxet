#!/usr/bin/env bash

set -e

cxxet_include scripts/common/docker_image_name
cxxet_include scripts/common/docker_validate_image_name_base
cxxet_include scripts/common/ensure_docker_is_allowed
cxxet_include scripts/common/list_dockerfiles
cxxet_include scripts/common/reject_further_args

function docker_devcontainer() {
    ensure_docker_is_allowed

    function usage() {
        if [[ "$1" != '--short' ]]; then
            printf 'docker_devcontainer: generate devcontainer config for given image\n' >&2
        fi
        printf 'Usage: docker_devcontainer <image_name_base|-h|--help>\n' >&2
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

    local image_name="$(docker_image_name "${image_name_base}")"
    local devcontainer_dir="${CXXET_ROOT_DIR}/.devcontainer/${image_name_base}"
    local devcontainer_file="${devcontainer_dir}/devcontainer.json"

    mkdir -p "${devcontainer_dir}"

    # Generate devcontainer.json
    cat > "${devcontainer_file}" << EOF
{
    "name": "${image_name_base}",
    "image": "${image_name}",
    "workspaceFolder": "${CXXET_ROOT_DIR}",
    "remoteUser": "$(id -un)",
    "containerUser": "$(id -un)",
    "runArgs": [
        "--user",
        "$(id -u):$(id -g)",
        "--hostname",
        "${image_name_base}_devcontainer",
    ],
    "mounts": [
        "source=${CXXET_ROOT_DIR},target=${CXXET_ROOT_DIR},type=bind,consistency=cached",
        "source=${HOME}/.gitconfig,target=/home/$(id -un)/.gitconfig,type=bind,consistency=cached,readonly",
        "source=${HOME}/.ssh,target=/home/$(id -un)/.ssh,type=bind,consistency=cached,readonly"
    ],
    "customizations": {
        "vscode": {
            "extensions": [
                "llvm-vs-code-extensions.vscode-clangd",
                "twxs.cmake",
                "cheshirekow.cmake-format",
                "eamodio.gitlens",
                "jetmartin.bats",
                "DavidAnson.vscode-markdownlint",
                "ClemensPeters.format-json",
                "foxundermoon.shell-format",
                "redhat.vscode-xml",
                "redhat.vscode-yaml"
            ]
        }
    },
    "postCreateCommand": "printf 'Development container ready!\n'",
    "shutdownAction": "stopContainer"
}
EOF

    printf 'Generated devcontainer configuration at: %s\n' "${devcontainer_file}"
    printf 'Image: %s\n' "${image_name}"
}
