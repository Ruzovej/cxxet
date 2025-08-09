#!/usr/bin/env bash

set -e

function purge_artifacts() {
    rm -rf \
        "${CXXET_ROOT_DIR}/bin" \
        "${CXXET_ROOT_DIR}/build" \
        "${CXXET_ROOT_DIR}/compile_commands.json"
}
