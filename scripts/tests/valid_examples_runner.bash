#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function valid_examples_runner() {
    local preset="${1:-release}"

    printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n' >&2
    printf -- '-=-=-=-=-=-=-=- Building needed targets (with preset %s) for examples:\n' "${preset}" >&2
    compile \
        --preset "${preset}" \
        --target cxxet_examples \
        --polite-ln-compile_commands >&2

    local bin_dir="${CXXET_ROOT_DIR}/bin/${preset}"
    local target_dir="${CXXET_ROOT_DIR}/tmp/$(date +%Y-%m-%d_%H-%M-%S)"
    mkdir -p "${target_dir}"

    printf -- '-=-=-=-=-=-=-=- Executing %s examples:\n' "${preset}" >&2
    time (
        set -x

        export CXXET_DEFAULT_BLOCK_SIZE=3 # torture it with some non-default value & force it to allocate more during the run
        "${bin_dir}/cxxet_example_complete_1" "${target_dir}/easy_complete_1.json"
        "${bin_dir}/cxxet_example_counter_1" "${target_dir}/easy_counter_1.json"
        "${bin_dir}/cxxet_example_counter_2" "${target_dir}/easy_counter_2.json"
        "${bin_dir}/cxxet_example_duration_1" "${target_dir}/easy_duration_1.json"
        "${bin_dir}/cxxet_example_instant_1" "${target_dir}/easy_instant_1.json"
        "${bin_dir}/cxxet_example_instant_2" "${target_dir}/easy_instant_2.json"
        "${bin_dir}/cxxet_example_local_file_sink_1" "${target_dir}/easy_local_sink_1_a.json" "${target_dir}/easy_local_sink_1_b.json"
        "${bin_dir}/cxxet_example_local_file_sink_2" "${target_dir}/easy_local_sink_2_a.json" "${target_dir}/easy_local_sink_2_b.json"
        "${bin_dir}/cxxet_example_local_file_sink_3" "${target_dir}/easy_local_sink_3_a.json" "${target_dir}/easy_local_sink_3_b.json"
        "${bin_dir}/cxxet_example_local_file_sink_4" "${target_dir}/easy_local_sink_4.json"
    )
    printf '\n' >&2
}
