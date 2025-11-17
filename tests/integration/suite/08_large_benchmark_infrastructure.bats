#!/usr/bin/env bats

load "${BATS_HELPER_DIRECTORY}/bats-assert/load"
load "${BATS_HELPER_DIRECTORY}/bats-support/load"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/adjust_cxxet_env_variables"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/populate_CXXET_BIN_DIR"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/prepare_CXXET_RESULTS_DIR"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/refute_sanitizer_output"
load "${CUSTOM_BATS_HELPERS_DIRECTORY}/user_log"

function setup_file() {
    adjust_cxxet_env_variables

    populate_CXXET_BIN_DIR

    prepare_CXXET_RESULTS_DIR
}

function setup() {
    :
}

function teardown() {
    :
}

function teardown_file() {
    :
}

@test "Large benchmark correctness test 1 (cxxet_bench_mt_counter)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_1"
    local num_iters=50
    local num_threads=4
    local executable="${CXXET_BIN_DIR}/cxxet_bench_mt_counter"

    local args=(
        "${num_iters}"
        2 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "$(( num_iters * num_threads ))"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "C")] | length' "${output_file2}")" "$(( num_iters * num_threads ))"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_driver_statistics="$(( 2 + 4 * num_extracted_statistic_from_real_file_low ))" # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta` + `examples/for_benchmarks/postprocessing/src/result_processor.cxx` -> function `process_benchmark_thread_perfs` ... 6 items, only 4 of them have (7) detailed statistics
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_high ))"
    # first `*2`: `cxxet` + `bare`
    # observing one thing - gaps (or intervals ...) - in the results:
    # there are 4 * 49 gaps -> detailed statistics will consist of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_high ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_counter_marker_interval"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 2 (cxxet_bench_st_instant)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_2"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_instant"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low ))"
    # first `*2`: `cxxet` + `bare`
    # observing one thing - gaps (or intervals ...) - in the results:
    # there are 4 * 49 gaps -> detailed statistics will consist of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_instant_marker_interval"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 3 (cxxet_bench_st_guarded_instant)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_3"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_guarded_instant"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "$(( num_iters * 2 ))"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "i")] | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high * 2 ))"
    # first `*2`: `cxxet` + `bare`
    # observing three things ...
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high * 2 ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_guarding_complete_marker_duration"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_guarding_complete_marker_gap"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_guarded_instant_dist_from_complete_center"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 4 (cxxet_bench_st_complete)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_4"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_complete"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "X")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    # first `*2`: `cxxet` + `bare`
    # observing one thing - durations - in the results:
    # there are 50 durations -> detailed statistics will consist of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_complete_marker_duration"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_complete_marker_gap"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark correctness test 5 (cxxet_bench_st_duration)" {
    local result_base="${CXXET_RESULTS_DIR}/example_large_benchmark_5"
    local num_iters=50
    local num_threads=1
    local executable="${CXXET_BIN_DIR}/cxxet_bench_st_duration"

    local args=(
        "${num_iters}"
        1 # marker_after_iter
        3 # cxxet_reserve_buffer
        "${num_threads}"
        1 # repetition
        1 # num_repetitions
        "${result_base}" # bench_result_filename_base
    )

    local meta_file1="${result_base}_bare_meta.json"
    local output_file1="${result_base}_bare.json"
    run "${executable}_bare" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file1}" ]
    refute [ -f "${output_file1}" ]

    assert_equal "$(jq -e 'length' "${meta_file1}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file1}")" "\"${executable}_bare\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file1}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file1}")" '"bare"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file1}")" "${num_threads}"

    local meta_file2="${result_base}_meta.json"
    local output_file2="${result_base}.json"
    run "${executable}" "${args[@]}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${meta_file2}" ]
    assert [ -f "${output_file2}" ]

    assert_equal "$(jq -e 'length' "${meta_file2}")" 2
    assert_equal "$(jq -e '.meta_info.benchmark_executable' "${meta_file2}")" "\"${executable}\""
    assert_equal "$(jq -e '.meta_info.benchmark_name' "${meta_file2}")" "\"${executable##*/}\""
    assert_equal "$(jq -e '.meta_info.traced' "${meta_file2}")" '"cxxet"'
    assert_equal "$(jq -e '.thread_perfs | length' "${meta_file2}")" "${num_threads}"
    assert_equal "$(jq -e '.traceEvents | length' "${output_file2}")" "$(( num_iters * 2 ))"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "B")] | length' "${output_file2}")" "${num_iters}"
    assert_equal "$(jq -e '[.traceEvents[] | select(.ph == "E")] | length' "${output_file2}")" "${num_iters}"

    local pp_file="${result_base}_pp.json"
    run "${CXXET_BIN_DIR}/cxxet_large_bench_postprocess" \
        --git-commit-hash-file "${CXXET_RESULTS_DIR}/../../commit_hash.json" \
        --out-json "${pp_file}" \
        "${meta_file1}" "${meta_file2}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${pp_file}" ]
    # TODO for the test cases above too!

    assert_equal "$(jq -e 'length' "${pp_file}")" 2

    local num_driver_statistics=6 # see `examples/for_benchmarks/common/sources/cxxet_bench_driver.cxx` -> `struct meta`
    local num_extracted_statistic_from_real_file_low=7 # `examples/for_benchmarks/postprocessing/include/statistics.hxx` -> method `percentiles_near_min_max_meaningful`
    local num_extracted_statistic_from_real_file_high=9
    assert_equal "$(jq -e '.benchmarks | length' "${pp_file}")" "$(( num_driver_statistics * 2 + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    # first `*2`: `cxxet` + `bare`
    # observing two things - durations and gaps - in the results:
    # there are 49 gaps and 50 durations ... -> detailed statistics for one will consist of 7 entries, for the other of 9 entries
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "bare")] | length' "${pp_file}")" "${num_driver_statistics}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.used_lib == "cxxet")] | length' "${pp_file}")" "$(( num_driver_statistics + num_extracted_statistic_from_real_file_low + num_extracted_statistic_from_real_file_high ))"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_duration_marker_gap"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_low}"
    assert_equal "$(jq -e '[.benchmarks[].benchmark_params | select(.subtype | startswith("TRACE_duration_marker_duration"))] | length' "${pp_file}")" "${num_extracted_statistic_from_real_file_high}"
    # IMHO unnecessary to test it in more detail ...
}

@test "Large benchmark result comparison test" {
    local executable="${CXXET_BIN_DIR}/cxxet_large_bench_compare"
    local fake_baseline="${CXXET_ROOT_DIR}/tests/integration/fake_data/large_bench_fake_res_1.json"
    local fake_challenger="${CXXET_ROOT_DIR}/tests/integration/fake_data/large_bench_fake_res_2.json"
    local results="${CXXET_RESULTS_DIR}/large_benchmark_compare_output.json"

    run "${executable}" --verbose --out-json "${results}" "${fake_baseline}" "${fake_challenger}"
    assert_success
    refute_sanitizer_output
    assert [ -f "${results}" ]

    # TODO (https://github.com/Ruzovej/cxxet/issues/188): `sub_fake_common_baseline_better` and `sub_fake_common_challenger_better` shouldn't come out as `EQUIVALENT`!!!
    assert_output --partial "No challenger data for measurement fake_1 (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_1, impl=cxxet); skipping comparison"
    assert_output --partial "No baseline data for measurement fake_2 (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_2, impl=cxxet); skipping comparison"
    assert_output --partial "Challenger is most probably EQUIVALENT to baseline for measurement fake_common (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_common_baseline_better, impl=cxxet)"
    assert_output --partial "Challenger is most probably EQUIVALENT to baseline for measurement fake_common (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_common_challenger_better, impl=cxxet)"
    assert_output --partial "Challenger is most probably EQUIVALENT to baseline for measurement fake_common (crb=3, mai=2, nit=50, nths=4, subtype=sub_fake_common_eq, impl=cxxet)"

    # ditto:
    assert_output --partial "  Total measurements compared: 5"
    assert_output --partial "  Missing baseline measurements: 1 (20.000000 %)"
    assert_output --partial "  Missing challenger measurements: 1 (20.000000 %)"
    assert_output --partial "  Draws: 3 (60.000000 %)"
    assert_output --partial "  Challenger better: 0 (0.000000 %)"
    assert_output --partial "  Challenger worse: 0 (0.000000 %)"

    assert_output --partial "Saved comparison results into ${results}"
}
