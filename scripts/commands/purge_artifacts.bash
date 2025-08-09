#!/usr/bin/env bash

set -e

function purge_artifacts() {
    rm -rf \
        "${CXXET_ROOT_DIR}/bin" \
        "${CXXET_ROOT_DIR}/build" \
        "${CXXET_ROOT_DIR}/compile_commands.json" \
        "${CXXET_ROOT_DIR}/tests/integration/external" \
        "${CXXET_ROOT_DIR}/graphviz" \
        "${CXXET_ROOT_DIR}/tmp"
    # TODO is it enough? Or even more ... e.g. everything that is listed in (any) `.gitignore`?!
}
