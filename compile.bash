#!/usr/bin/env bash

set -e

cxxet_preset=tsan
targets=()
defines=()
force_compile_commands_symlink='true'

while (( $# > 0 )); do
    case "$1" in
        --preset)
            cxxet_preset="$2"
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

num_jobs="$(nproc)"
(
    set -x
    cmake \
        -S . \
        -B "build/${cxxet_preset}" \
        "${defines[@]}" \
        --preset "${cxxet_preset}" #\
        # --graphviz="graphviz/${cxxet_preset}"
)

[[ "${force_compile_commands_symlink}" == 'false' && -f compile_commands.json ]] \
|| (
    set -x
    ln \
        --symbolic \
        --force \
        "build/${cxxet_preset}/compile_commands.json" \
        compile_commands.json
)

(
    set -x
    cmake \
        --build "build/${cxxet_preset}" \
        -j "${num_jobs}" \
        "${targets[@]}"
)
