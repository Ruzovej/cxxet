#!/usr/bin/env bash

set -e

case "$1" in
    -b|--bats)
        source tests/integration/bats_runner.bash
        bats_runner
        ;;
    *)
        printf 'Unknown option: %s\n' "$1"
        exit 1
        ;;
esac
