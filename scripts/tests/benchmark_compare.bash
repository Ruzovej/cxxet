#!/usr/bin/env bash

set -e

function benchmark_compare() {
    cxxet_require_command python3 # TODO how to check for presence of `venv` module?!

    local benchmark_help='false'
    local benchmark_args=()
    local out_file
    local file1
    local file2

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'benchmark_compare: compare micro-benchmark results\n'
            fi
            printf 'Usage: benchmarks compare <[options...] <file1> <file2>|-h|--help|--benchmark-help>\n'
            printf '    --help, -h            Show this help message\n'
            printf '    --benchmark-help      Show help message of the benchmark comparison script\n'
            printf 'Where options are:\n'
            printf '    --benchmark_*, --v=*  Pass any such flag directly to the benchmark runner\n'
            printf '    --out-json, -o OUT    Equivalent to "--dump_to_json OUT"\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            --benchmark-help)
                benchmark_help='true'
                shift
                ;;
            --benchmark_*|--v=*)
                benchmark_args+=("$1")
                shift
                ;;
            --out-json|-o)
                benchmark_args+=("--dump_to_json ${2:?No output file specified!}")
                out_file="${2:?No output file specified!}"
                shift 2
                ;;
            --help|-h)
                usage
                return 0
                ;;
            *)
                # this doesn't prevent parsing further flags in between those 2 files ... ignoring it for now
                if [[ -z "${file1}" ]]; then
                    file1="$1"
                elif [[ -z "${file2}" ]]; then
                    file2="$1"
                else
                    printf 'Unknown option: %s\n' "$1" >&2
                    usage --short
                    exit 1
                fi
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

        # comparison (https://github.com/google/benchmark/blob/main/docs/tools.md):

        if [[ "${benchmark_help}" == 'true' ]]; then
            (
                set -x
                "${script_dir}/compare.py" --help
            )
            return 0
        fi

        set -x
        "${script_dir}/compare.py" --display_aggregates_only "${benchmark_args[@]}" benchmarks "${file1}" "${file2}"
    ) >&2
}
