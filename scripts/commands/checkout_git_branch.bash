#!/usr/bin/env bash

set -e

function checkout_git_branch() {
    cxxet_require_command git

    local desired_branch=main

    function usage() {
        {
            if [[ "$1" != '--short' ]]; then
                printf 'checkout_git_branch: switch to "main" or new git branch, while pruning deleted branches\n'
            fi
            printf 'Usage: checkout_git_branch [options...]\n'
            printf 'Where options are:\n'
            printf '    --branch, -b BRANCH  Checkout (and potentially create) this branch (default: %s)\n' "${desired_branch}"
            printf '    --help, -h           Show this help message\n'
        } >&2
    }

    while (( $# > 0 )); do
        case "$1" in
            --branch|-b)
                desired_branch="${2:?No branch specified!}"
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

    if [[ "$(git -C "${CXXET_ROOT_DIR}" branch --show-current)" != "main" ]]; then
        git -C "${CXXET_ROOT_DIR}" fetch --prune origin main:main

        git -C "${CXXET_ROOT_DIR}" checkout main
    fi

    local branch
    for branch in $(git -C "${CXXET_ROOT_DIR}" branch -vv | grep ': gone]' | awk '{print $1}'); do
        git -C "${CXXET_ROOT_DIR}" branch -D "${branch}"
    done

    if [[ -n "${desired_branch}" && "${desired_branch}" != 'main' ]]; then
        git -C "${CXXET_ROOT_DIR}" checkout -b "${desired_branch}"
    fi
}
