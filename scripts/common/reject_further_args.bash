#!/usr/bin/env bash

set -e

function reject_further_args() {
    if (( $# > 0 )); then
        printf 'Error: This command does not accept any further arguments.\n' >&2
        return 1
    fi
}
