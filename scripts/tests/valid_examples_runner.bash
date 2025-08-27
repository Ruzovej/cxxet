#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function valid_examples_runner() {
    # don't "test" all reasonable presets but only single one:
    local default_preset=asan_d
    local preset="${1:-${default_preset}}"

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'valid_examples_runner: run example programs with specified preset\n'
            fi
            printf 'Usage: tests examples [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Run examples with the specified preset (default: %s)\n' "${default_preset}"
            printf '    --help, -h                 Show this help message\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            -p|--preset)
                preset="${2:?No preset specified!}"
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
    local target_dir="${CXXET_ROOT_DIR}/tmp/$(date +%Y-%m-%dT%H-%M-%S)_examples/${preset}"
    mkdir -p "${target_dir}"

    printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for examples:\n' "${preset}" >&2
    compile \
        --quiet \
        --preset "${preset}" \
        --target cxxet_examples \
        --ignore-compile_commands >&2

    printf -- '-=-=-=-=-=-=-=- Executing %s examples:\n' "${preset}" >&2
    (
        set -x

        export CXXET_DEFAULT_BLOCK_SIZE=3 # torture it with some non-default value & force it to allocate more during the run
        "${bin_dir}/cxxet_example_complete_1" "${target_dir}/easy_complete_1.json"
        "${bin_dir}/cxxet_example_counter_1" "${target_dir}/easy_counter_1.json"
        "${bin_dir}/cxxet_example_counter_2" "${target_dir}/easy_counter_2.json"
        "${bin_dir}/cxxet_example_duration_1" "${target_dir}/easy_duration_1.json"
        "${bin_dir}/cxxet_example_instant_1" "${target_dir}/easy_instant_1.json"
        "${bin_dir}/cxxet_example_instant_2" "${target_dir}/easy_instant_2.json"
        "${bin_dir}/cxxet_example_metadata_1" "${target_dir}/easy_metadata_1.json"
        "${bin_dir}/cxxet_example_categories_1" "${target_dir}/easy_categories_1.json"
        "${bin_dir}/cxxet_example_local_file_sink_1" "${target_dir}/easy_local_sink_1_a.json" "${target_dir}/easy_local_sink_1_b.json"
        "${bin_dir}/cxxet_example_local_file_sink_2" "${target_dir}/easy_local_sink_2_a.json" "${target_dir}/easy_local_sink_2_b.json"
        "${bin_dir}/cxxet_example_local_file_sink_3" "${target_dir}/easy_local_sink_3_a.json" "${target_dir}/easy_local_sink_3_b.json"
        "${bin_dir}/cxxet_example_local_file_sink_4" "${target_dir}/easy_local_sink_4.json"
        "${bin_dir}/cxxet_example_local_file_sink_5" > "${target_dir}/easy_local_sink_5.log"
    )
    printf '\n' >&2
}
