#!/usr/bin/env bash

function get_bats_package() {
    cxxet_require_command curl

    local base_dir="${1:?Missing target dir!}"
    local package_name="${2:?Missing package name!}"
    local package_version="${3:?Missing package version!}"

    local target_dir="${base_dir}/${package_name}"

    local version_file="${target_dir}/package_version.STAMP"
    if [[ -f "${version_file}" ]] \
        && [[ "$(cat "${version_file}")" == "${package_version}" ]]; then
        return 0;
    fi

    rm -rf "${target_dir}" "${version_file}"
    mkdir -p "${base_dir}"
    (
        set -eo pipefail
        printf "Getting %s %s\n" "${package_name}" "${package_version}"
        curl -sSfL \
            "https://github.com/bats-core/${package_name}/archive/refs/tags/v${package_version}.tar.gz" \
            | tar xz
        mv "${package_name}-${package_version}" "${target_dir}"
        printf "%s" "${package_version}" > "${version_file}"
    )
}
