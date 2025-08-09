#!/usr/bin/env bash

set -e

function cxxet_list_files() {
    local prefix="${1:?}"
    local suffix="${2:?}"

    printf 'Possible options are:\n' >&2
    local candidate_file
    for candidate_file in "${prefix}/"*."${suffix}"; do
        printf ' - %s\n' "$(basename "${candidate_file}" ".${suffix}")" >&2
    done
}
export -f cxxet_list_files
