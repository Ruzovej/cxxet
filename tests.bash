#!/usr/bin/env bash

set -e

case "$1" in
    -b|--bats)
        shift
        source tests/integration/bats_runner.bash
        bats_runner
        ;;
    *)
        printf 'Unknown option: %s\n' "$1"
        exit 1
        ;;
esac

if [[ "$#" -gt 0 ]]; then
    printf 'Ignoring further command line args (%s)\n' "$*"
    exit 1
fi
