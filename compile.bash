#!/usr/bin/env bash

set -e

while (( $# > 0 )); do
    case "$1" in
        --preset)
            rsm_preset_arg="$2"
            shift 2
            ;;
        --target)
            target="$2"
            shift 2
            ;;
        *)
            printf 'Unknown option: %s\n' "$1"
            exit 1
            ;;
    esac
done

rsm_preset="${rsm_preset_arg:-tsan}"
num_jobs="$(nproc)"
(
    set -x
    cmake \
        -S . \
        -B "build/${rsm_preset}" \
        --graphviz="graphviz/${rsm_preset}" \
        --preset "${rsm_preset}"
    ln \
        --symbolic \
        --force \
        "build/${rsm_preset}/compile_commands.json" \
        compile_commands.json
    cmake \
        --build "build/${rsm_preset}" \
        -j "${num_jobs}" \
        --target "${target:-all}"
)
