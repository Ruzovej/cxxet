#!/usr/bin/env bash

set -e

targets=()
force_compile_commands_symlink='true'
defines=()

while (( $# > 0 )); do
    case "$1" in
        --preset)
            rsm_preset_arg="$2"
            shift 2
            ;;
        --target)
            targets+=("$1" "$2")
            shift 2
            ;;
        -D*)
            defines+=("$1")
            shift 1
            ;;
        --polite-ln-compile_commands)
            force_compile_commands_symlink='false'
            shift 1
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
        "${defines[@]}" \
        --graphviz="graphviz/${rsm_preset}" \
        --preset "${rsm_preset}"
    [[ "${force_compile_commands_symlink}" == 'false' \
        && -f compile_commands.json ]] \
        || \
        ln \
            --symbolic \
            --force \
            "build/${rsm_preset}/compile_commands.json" \
            compile_commands.json
    cmake \
        --build "build/${rsm_preset}" \
        -j "${num_jobs}" \
        "${targets[@]}"
)
