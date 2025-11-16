#!/usr/bin/env bash

function skip_for_preset_other_than() {
    if [[ "${CXXET_PRESET}" != "${1:?}" ]]; then
        skip "this should test only '$1' build(s), current preset is '${CXXET_PRESET}'"
    fi
}
