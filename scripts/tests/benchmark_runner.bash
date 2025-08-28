#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function benchmark_runner() {
    # don't "test" all reasonable presets but only single one:
    local default_preset=tsan
    local preset="${default_preset}"
    local benchmark_help='false'
    local benchmark_args=()

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'benchmark_runner: run benchmarks with specified preset\n'
            fi
            printf 'Usage: tests benchmark [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Run benchmarks with the specified preset (default: %s)\n' "${default_preset}"
            printf '    --benchmark-help           Show help message of the benchmark runner\n'
            printf '    --benchmark*, --v=*        Pass any such flag directly to the benchmark runner\n'
            printf '    --help, -h                 Show this help message\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            -p|--preset)
                preset="${2:?No preset specified!}"
                shift 2
                ;;
            --benchmark-help)
                benchmark_help='true'
                shift
                ;;
            --benchmark*|--v=*)
                benchmark_args+=("$1")
                shift
                ;;
            --help|-h)
                usage
                return 0
                ;;
            *)
                printf 'Unknown option: %s\n' "$1" >&2
                usage --short
                exit 1
                ;;
        esac
    done

    local bin_dir="${CXXET_ROOT_DIR}/bin/${preset}"
    local target_dir="${CXXET_ROOT_DIR}/tmp/$(date +%Y-%m-%dT%H-%M-%S)_benchmarks/${preset}"
    mkdir -p "${target_dir}"

    printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for benchmarks:\n' "${preset}" >&2
    compile \
        --quiet \
        --preset "${preset}" \
        --target cxxet_benchmarks \
        --ignore-compile_commands >&2

    if [[ "${benchmark_help}" == 'true' ]]; then
        (
            set -x
            "${bin_dir}/cxxet_benchmarks" --help
        ) >&2
        return 0
    fi

    printf -- '-=-=-=-=-=-=-=- Executing %s benchmarks:\n' "${preset}" >&2
    (
        set -x
        "${bin_dir}/cxxet_benchmarks" "${benchmark_args[@]}" >&2
    )
    printf '\n' >&2
}
