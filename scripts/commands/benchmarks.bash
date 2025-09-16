#!/usr/bin/env bash

set -e

cxxet_include scripts/benchmarks/runner/micro
cxxet_include scripts/tests/benchmark_compare

function benchmarks() {
    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'benchmarks: perform or evaluate various benchmark(s)\n'
            fi
            printf 'Usage: benchmarks option [args ...]\n'
            printf 'Where option is one of:\n'
            printf '    -c, --compare, compare  Compare 2 micro-benchmark results (using 3rd party (google benchmark) python3 script)\n'
            printf '    -m, --micro, micro      Run C++ implementation micro-benchmarks\n'
            printf '    --help, -h              Show this help message\n'
            printf 'Remaining args are passed to the previously parsed "option", e.g. for suite-specific details/help, pass -h|--help to it.\n'
        } >&2
    }

    case "$1" in
        -c|--compare|compare)
            shift
            benchmark_compare "$@"
            ;;
        -m|--micro|micro)
            shift
            micro "$@"
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
