#!/usr/bin/env bash

set -e

case "$1" in
    -b|--bats)
        shift
        source tests/integration/bats_runner.bash
        bats_runner "$@"
        ;;
    -u|--unit)
        shift
        source tests/unit/unit_runner.bash
        unit_runner "$@"
        ;;
    *)
        printf 'Unknown option: %s\n' "$1"
        exit 1
        ;;
esac
