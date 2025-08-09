#!/usr/bin/env bash

set -e

cxxet_include scripts/tests/bats_runner
cxxet_include scripts/tests/unit_runner
cxxet_include scripts/tests/valid_examples_runner

function tests() {
    case "$1" in
        -a|--all)
            unit_runner
            valid_examples_runner
            bats_runner
            ;;
        -b|--bats)
            shift
            bats_runner "$@"
            ;;
        -e|--examples)
            shift
            valid_examples_runner "$@"
            ;;
        -u|--unit)
            shift
            unit_runner "$@"
            ;;
        *)
            printf 'Unknown option "%s"\n' "$1" >&2
            exit 1
            ;;
    esac
}
