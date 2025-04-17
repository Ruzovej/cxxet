#!/usr/bin/env bash

function initialize_bats() {
    (
        source tests/integration/init/get_bats_package.bash

        get_bats_package \
            tests/integration/external \
            bats-core \
            1.11.0

        get_bats_package \
            tests/integration/external/bats-helper \
            bats-support \
            0.3.0

        get_bats_package \
            tests/integration/external/bats-helper \
            bats-assert \
            2.1.0
    )

    export BATS_CORE_DIRECTORY="${PWD}/tests/integration/external/bats-core"
    export BATS_HELPER_DIRECTORY="${PWD}/tests/integration/external/bats-helper"
    #export BATS_EXTENSIONS_DIRECTORY="${PWD}/tests/integration/helper/bats-extensions"

    export BATS_EXECUTABLE="${BATS_CORE_DIRECTORY}/bin/bats"
}
