#!/usr/bin/env bash

cxxet_include scripts/common/docker_image_name
cxxet_include scripts/common/ensure_docker_is_allowed

function docker_run() {
    ensure_docker_is_allowed

    local user_id="$(id -u)"
    local group_id="$(id -g)"

    local docker_run_args=()
    local tail_args=()

    while (( $# > 0 )); do
        case "$1" in
            --interactive|-i)
                docker_run_args+=("--interactive")
                ;;
            --tty|-t)
                docker_run_args+=("--tty")
                ;;
            --hostname|-h)
                docker_run_args+=("--hostname" "${2:?}")
                shift
                ;;
            *)
                tail_args+=("$1")
                ;;
        esac
        shift
    done

    (
        set -x
        docker run \
            "${docker_run_args[@]}" \
            --rm \
            --user "${user_id}:${group_id}" \
            --volume "${CXXET_ROOT_DIR}:${CXXET_ROOT_DIR}" \
            --workdir "${CXXET_ROOT_DIR}" \
            "${tail_args[@]}"
    )
}
