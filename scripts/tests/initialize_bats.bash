#!/usr/bin/env bash

cxxet_include scripts/tests/get_bats_package

function initialize_bats() {
    get_bats_package \
        "${CXXET_ROOT_DIR}/tests/integration/external" \
        bats-core \
        1.12.0

    get_bats_package \
        "${CXXET_ROOT_DIR}/tests/integration/external/bats-helper" \
        bats-support \
        0.3.0

    get_bats_package \
        "${CXXET_ROOT_DIR}/tests/integration/external/bats-helper" \
        bats-assert \
        2.1.0

    export CUSTOM_BATS_HELPERS_DIRECTORY="${CXXET_ROOT_DIR}/tests/integration/custom_helpers"
    export BATS_CORE_DIRECTORY="${CXXET_ROOT_DIR}/tests/integration/external/bats-core"
    export BATS_HELPER_DIRECTORY="${CXXET_ROOT_DIR}/tests/integration/external/bats-helper"
    #export BATS_EXTENSIONS_DIRECTORY="${CXXET_ROOT_DIR}/tests/integration/helper/bats-extensions"

    export BATS_EXECUTABLE="${BATS_CORE_DIRECTORY}/bin/bats"
}
