#!/usr/bin/env bash

set -e

cxxet_include scripts/tests/bats_runner
cxxet_include scripts/tests/unit_runner

function tests() {
    case "$1" in
        -b|--bats)
            shift
            bats_runner "$@"
            ;;
        -u|--unit)
            shift
            unit_runner "$@"
            ;;
        *)
            printf 'Unknown option: %s\n' "$1"
            exit 1
            ;;
    esac
}
