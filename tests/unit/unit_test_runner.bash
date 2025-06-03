#!/usr/bin/env bash

function unit_test_runner() {
    local num_rounds=1

    while (($# > 0)); do
        case "$1" in
        -r | --rounds)
            shift
            num_rounds="$1"
            ;;
        *)
            break
            ;;
        esac
        shift
    done

    local test_presets=(
        asan
        asan_d
        tsan
        tsan_d
        release
    )

    local round=1
    while ((round <= num_rounds)); do
        [[ "${num_rounds}" -eq 1 ]] ||
            printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Executing unit tests round no. %s/%s:\n\n' "${round}" "${num_rounds}"

        local preset
        for preset in "${test_presets[@]}"; do
            printf -- '-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- Executing %s unit tests:\n' "${preset}"
            if [[ "${round}" -eq 1 ]]; then
                ./compile.bash \
                    -DCXXST_BUILD_TESTS=ON \
                    --preset "${preset}" \
                    --target rsm_unit_tests \
                    --polite-ln-compile_commands
            fi
            "bin/${preset}/rsm_unit_tests" \
                --no-intro
            printf '\n'
        done

        ((round++))
    done
}
