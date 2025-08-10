#!/usr/bin/env bash

set -e

function cxxet_require_command() {
    while (( $# > 0 )); do
        local command_name="$1"
        if ! command -v "${command_name}" &> /dev/null; then
            printf 'Error: "%s" is not installed or not in PATH\n' "${command_name}" >&2
            exit 1
        fi
        shift
    done
    return 0
}
export -f cxxet_require_command
