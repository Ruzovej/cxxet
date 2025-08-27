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
    local quiet_flag=('--quiet')
    local redundant_flag=()

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
            printf '    --verbose, -v              Increase verbosity (by not suppressing stdout)\n'
            printf '    --redundant, -r            Pass --redundant flag to abidiff\n'
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
            --verbose|-v)
                quiet_flag=()
                shift 1
                ;;
            --redundant|-r)
                redundant_flag+=('--redundant')
                shift 1
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
        # It can be observed that this is what's needed by running e.g.
        # `$ git log --format=oneline -n 1 main` ...
        prev_commit="$(git rev-parse "${prev_branch}")"
    fi

    # save (some) space by reusing the same folder in subsequent runs:
    local abi_check_repo_dir="${CXXET_ROOT_DIR}/tmp/abi_check_repo"
    local baseline_so_name="${abi_check_repo_dir}/bin/${preset}/libcxxet.so"
    (
        if [[ ! -d "${abi_check_repo_dir}" ]]; then
            mkdir -p "${abi_check_repo_dir}"
            (
                set -x
                git clone \
                    --branch "${prev_branch}" \
                    "${quiet_flag[@]}" \
                    "file://${CXXET_ROOT_DIR}" \
                    "${abi_check_repo_dir}"
            )
        else
            (
                set -x
                git -C "${abi_check_repo_dir}" fetch \
                    "${quiet_flag[@]}" \
                    origin \
                    "${prev_branch}"
            )
        fi
        (
            set -x
            git -C "${abi_check_repo_dir}" checkout \
                "${quiet_flag[@]}" \
                --detach \
                "${prev_commit}"
        )

        cd "${abi_check_repo_dir}" # Needed so the `cxxet_manage.bash` properly determines its root directory:
        ./cxxet_manage.bash compile \
            --preset "${preset}" \
            "${quiet_flag[@]}" \
            --ignore-compile_commands
        
        [[ -f "${baseline_so_name}" ]] || {
            printf 'Error: Baseline shared object file "%s" does not exist!\n' "${baseline_so_name}" >&2
            exit 1
        }
    ) >&2

    local current_so_name="${CXXET_ROOT_DIR}/bin/${preset}/libcxxet.so"
    compile \
        --preset "${preset}" \
        "${quiet_flag[@]}" \
        --ignore-compile_commands

    [[ -f "${current_so_name}" ]] || {
        printf 'Error: Current shared object file "%s" does not exist!\n' "${current_so_name}" >&2
        exit 1
    }

    (
        set -x
        abidiff \
            --harmless \
            "${redundant_flag[@]}" \
            "${baseline_so_name}" \
            "${current_so_name}"
    ) >&2
}
