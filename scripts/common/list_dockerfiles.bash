#!/usr/bin/env bash

set -e

function list_dockerfiles() {
    cxxet_list_files "${CXXET_ROOT_DIR}/docker" Dockerfile
}
