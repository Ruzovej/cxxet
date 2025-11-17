#!/usr/bin/env bash

function populate_CXXET_BIN_DIR() {
    export CXXET_BIN_DIR="${CXXET_ROOT_DIR}/bin/${CXXET_PRESET}"
    # must have been already created, and should contain (not checked here ...) compiled binaries, etc.:
    assert [ -d "${CXXET_BIN_DIR}" ]
}
