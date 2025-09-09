#!/usr/bin/env bash

set -e

function benchmark_compare() {
    cxxet_require_command python3 # TODO how to check for presence of `venv` module?!

    local script_args=()

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'benchmark_compare: thin wrapper around script provided by google/benchmark library\n'
            fi
            printf 'Usage: benchmarks compare <[options...]|-h|--help>\n'
            printf '    --help, -h            Show this help message\n'
            printf '    --benchmark-help      Show help message of the benchmark comparison script\n'
            printf 'Where options are forwarded to the underlying script. For short, useful options are e.g.:\n'
            printf '    --dump_to_json FILE\n'
            printf '    --display_aggregates_only\n'
            printf 'For details, refer to the script documentation (https://github.com/google/benchmark/blob/main/docs/tools.md), or help message.\n'
            printf 'Examples of usage:\n'
            printf '  - compare 2 files:\n'
            printf '    benchmarks compare --display_aggregates_only benchmarks FILE1 FILE2'
            printf '  - compare different benchmarks from same file:\n'
            printf '    benchmarks compare --display_aggregates_only filters FILE FILTER1 FILTER2'
            printf '  - compare different benchmarks from different files:\n'
            printf '    benchmarks compare --display_aggregates_only benchmarksfiltered FILE1 FILTER1 FILE2 FILTER2'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            --benchmark-help)
                script_args+=('--help')
                shift
                ;;
            --help|-h)
                usage
                return 0
                ;;
            *)
                script_args+=("$1")
                shift 1
                ;;
        esac
    done

    local script_dir
    local possible_path_base
    for possible_path_base in "${CXXET_ROOT_DIR}/build/"*"/_deps/benchmark-src/tools"; do
        if [[ -f "${possible_path_base}/requirements.txt" ]]; then
            script_dir="${possible_path_base}"
            break
        fi
    done

    # python3 venv (https://docs.python.org/3/library/venv.html):
    local venv_dir="${CXXET_ROOT_DIR}/tmp/bench_compare_venv"
    local fresh_venv='false'
    if [[ ! -f "${venv_dir}/bin/activate" ]]; then
        mkdir -p "${venv_dir}"
        (
            set -x
            python3 -m venv --clear "${venv_dir}"
        ) >&2
        fresh_venv='true'
    fi

    (
        source "${venv_dir}/bin/activate"

        if [[ "${fresh_venv}" == 'true' ]]; then
            pip3 install -r "${script_dir}/requirements.txt"
        fi

        set -x
        "${script_dir}/compare.py" "${script_args[@]}"
    ) >&2
}
