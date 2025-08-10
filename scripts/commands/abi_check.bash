#!/usr/bin/env bash

set -e

cxxet_include scripts/commands/compile

function abi_check() {
    cxxet_require_command \
        abidiff \
        git

    local preset=abidiff
    local prev_commit
    local prev_branch=main

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'abi_check: compare abi of previous library build with current one\n'
            fi
            printf 'Usage: abi_check [options...]\n'
            printf 'Where options are:\n'
            printf '    --preset, -p PRESET        Set the CMake preset (default: %s)\n' "${preset}"
            printf '    --commit, -c COMMIT        Compare with the specified commit. If not specified, uses last one\n'
            printf '    --branch, -b BRANCH        Compare with given commit on specified branch (default: %s)\n' "${prev_branch}"
            printf '    --help, -h                 Show this help message\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            --preset|-p)
                preset="${2:?No preset specified!}"
                shift 2
                ;;
            --commit|-c)
                prev_commit="${2:?No commit specified!}"
                shift 2
                ;;
            --branch|-b)
                prev_branch="${2:?No branch specified!}"
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

    if [[ -z "${prev_commit}" ]]; then
        # It can be observed that this is what's needed by running e.g.:
        # `$ git log --format=oneline -n 1 main` ...
        prev_commit="$(git rev-parse "${prev_branch}")"
    fi

    # TODO new folder everytime (e.g. `mktemp`, ...), or save some space by reusing the same folder in subsequent runs?
    local abidiff_dir="${CXXET_ROOT_DIR}/tmp/cloned_repo_for_abidiff/preset_${preset}"
    local baseline_so_name="${abidiff_dir}/bin/${preset}/libcxxet.so"
    (
        #rm -rf "${abidiff_dir}" # TODO (https://github.com/Ruzovej/cxxet/issues/113) remove later - now it's here for testing purposes ...
        if [[ ! -d "${abidiff_dir}" ]]; then
            mkdir -p "${abidiff_dir}"
            (
                set -x
                git clone \
                    --branch "${prev_branch}" \
                    --quiet \
                    "file://${CXXET_ROOT_DIR}" \
                    "${abidiff_dir}"
            )
        else
            (
                set -x
                git -C "${abidiff_dir}" fetch \
                    --quiet \
                    origin \
                    "${prev_branch}"
            )
        fi
        (
            set -x
            git -C "${abidiff_dir}" checkout \
                --quiet \
                --detach \
                "${prev_commit}"
        )

        cd "${abidiff_dir}" # Needed so the `cxxet_manage.bash` properly determines its root directory:
        ./cxxet_manage.bash compile \
            --preset "${preset}" \
            --ignore-compile_commands
        
        [[ -f "${baseline_so_name}" ]] || {
            printf 'Error: Baseline shared object file "%s" does not exist!\n' "${baseline_so_name}" >&2
            exit 1
        }
    ) >&2

    # TODO (https://github.com/Ruzovej/cxxet/issues/113) implement `--quiet` option for `compile` subcommand & use it (at least) here
    local current_so_name="${CXXET_ROOT_DIR}/bin/${preset}/libcxxet.so"
    compile \
        --preset "${preset}" \
        --ignore-compile_commands >&2

    [[ -f "${current_so_name}" ]] || {
        printf 'Error: Current shared object file "%s" does not exist!\n' "${current_so_name}" >&2
        exit 1
    }

    (
        set -x
        abidiff \
            --harmless \
            --redundant \
            "${baseline_so_name}" \
            "${current_so_name}"
    ) >&2
}
