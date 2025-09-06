#!/usr/bin/env bash

set -e

cxxet_include scripts/tests/bats_runner
cxxet_include scripts/tests/unit_runner
cxxet_include scripts/tests/valid_examples_runner

function tests() {
    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'tests: run various test suites\n'
            fi
            printf 'Usage: tests option [args ...]\n'
            printf 'Where option is one of:\n'
            printf '    -a, --all, all              Run all test suites (unit, examples, bats)\n'
            printf '    -u, --unit, unit            Run unit tests\n'
            printf '    -e, --examples, examples    Run "valid" examples\n'
            printf '    -b, --bats, bats            Run bats tests\n'
            printf '    --help, -h                  Show this help message\n'
            printf 'Remaining args are passed to the previously parsed "option", e.g. for suite-specific details/help, pass -h|--help to it.\n'
        } >&2
    }

    case "$1" in
        -a|--all|all)
            shift
            unit_runner "$@"
            valid_examples_runner "$@"
            bats_runner "$@"
            ;;
        -b|--bats|bats)
            shift
            bats_runner "$@"
            ;;
        -e|--examples|examples)
            shift
            valid_examples_runner "$@"
            ;;
        -u|--unit|unit)
            shift
            unit_runner "$@"
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            printf 'Unknown option "%s"\n' "$1" >&2
            usage --short
            exit 1
            ;;
    esac
    return 0
}
