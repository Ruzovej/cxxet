#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function debug_executable() {
    cxxet_require_command gdb

    local preset=asan_d
    local target=cxxet_example_counter_2
    local default_block_size=4

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'debug_executable: executes given target under debugger\n'
            fi
            printf 'Usage: debug_executable [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET               Set the CMake preset (default: %s)\n' "${preset}"
            printf '    --target, -t TARGET               Set the target executable (default: %s)\n' "${target}"
            printf '    --default-block-size, -b SIZE     Set "CXXET_DEFAULT_BLOCK_SIZE" - the default block size (default: %s)\n' "${default_block_size}"
            printf '    --help, -h                        Show this help message\n'
        } >&2
    }


    while (( $# > 0 )); do
        case "$1" in
            --preset|-p)
                preset="${2:?No preset specified!}"
                shift 2
                ;;
            --target|-t)
                target="${2:?No target specified!}"
                shift 2
                ;;
            --default-block-size|-b)
                default_block_size="${2:?No default block size specified!}"
                shift 2
                ;;
            --help|-h)
                usage
                exit 0
                ;;
            *)
                printf 'Unknown option: %s\n' "$1" >&2
                usage --short
                exit 1
                ;;
        esac
    done

    compile \
        --preset "${preset}" \
        --target "${target}" \
        --ignore-compile_commands >&2

    (
        set -x
        export CXXET_DEFAULT_BLOCK_SIZE="${default_block_size}" # torture it with some non-default value & force it to allocate more during the run
        gdb --args "${CXXET_ROOT_DIR}/bin/${preset}/${target}"
        # TODO (https://github.com/Ruzovej/cxxet/issues/110) use `lldb`?! But leave possibility to use `gdb` too ...
    )
}
