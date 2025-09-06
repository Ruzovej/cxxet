#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function benchmark_runner() {
    # don't "test" all reasonable presets but only single one:
    local default_preset=tsan
    local preset="${default_preset}"
    local benchmark_help='false'
    local benchmark_args=()
    local out_file

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'benchmark_runner: run benchmarks with specified preset\n'
            fi
            printf 'Usage: benchmarks micro [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Run benchmarks with the specified preset (default: %s)\n' "${default_preset}"
            printf '    --benchmark-help           Show help message of the benchmark runner\n'
            printf '    --benchmark_*, --v=*       Pass any such flag directly to the benchmark runner\n'
            printf '    --interleaving, -i         Equivalent to "--benchmark_enable_random_interleaving=true"\n'
            printf '    --out-json, -o OUT         Equivalent to "--benchmark_out=OUT --benchmark_out_format=json"\n'
            printf '    --repetitions, -n N        Equivalent to "--benchmark_repetitions=N"\n'
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
            --benchmark_*|--v=*)
                benchmark_args+=("$1")
                shift
                ;;
            --interleaving|-i)
                benchmark_args+=('--benchmark_enable_random_interleaving=true')
                shift
                ;;
            --out-json|-o)
                out_file="${2:?No output file specified!}"
                benchmark_args+=("--benchmark_out=${out_file}" '--benchmark_out_format=json')
                shift 2
                ;;
            --repetitions|-n)
                benchmark_args+=("--benchmark_repetitions=${2:?No repetitions specified!}")
                shift 2
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
        "${bin_dir}/cxxet_benchmarks" "${benchmark_args[@]}"
    ) >&2
    printf '\n' >&2

    if [[ -f "${out_file}" ]]; then
        local temp_file="$(mktemp "${out_file}.XXXXXX")"
        local git_hash="$(git -C "${CXXET_ROOT_DIR}" rev-parse HEAD 2>/dev/null || printf "N/A")"
        local git_dirty="$(git -C "${CXXET_ROOT_DIR}" diff --shortstat)"

        jq --arg hash "${git_hash}${git_dirty:+ (dirty)}" '.context.cxxet_git_hash = $hash' "${out_file}" > "${temp_file}"
        mv "${temp_file}" "${out_file}"
    fi
}
