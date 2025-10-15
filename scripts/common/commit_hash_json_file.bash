#!/usr/bin/env bash

set -e

function commit_hash_json_file() {
    cxxet_require_command git

    local out_file="${1:?}/commit_hash.json"

    local git_hash="$(git -C "${CXXET_ROOT_DIR}" rev-parse HEAD 2>/dev/null || printf "N/A")"
    local git_dirty="$(git -C "${CXXET_ROOT_DIR}" diff --shortstat)"

    local result="${git_hash}${git_dirty:+ (dirty)}"

    printf '{"context":{"cxxet_git_hash":"%s"}}' "${result}" > "${out_file}"
}
