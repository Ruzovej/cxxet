add_custom_target(
    cxxet_examples
)

function(add_example target_name source_file)
    add_executable(
        ${target_name}
            ${source_file}
    )

    target_link_libraries(
        ${target_name}
            PRIVATE
            cxxet
    )

    add_executable(
        ${target_name}_bare
            ${source_file}
    )

    target_link_libraries(
        ${target_name}_bare
            PRIVATE
            cxxet_bare
    )

    add_dependencies(
        cxxet_examples
            ${target_name}
            ${target_name}_bare
    )
endfunction()

add_example(
    cxxet_example_instant_1
        "instant/1.cxx"
)

add_example(
    cxxet_example_instant_2
        "instant/2.cxx"
)

add_example(
    cxxet_example_counter_1
        "counter/1.cxx"
)

add_example(
    cxxet_example_counter_2
        "counter/2.cxx"
)

add_example(
    cxxet_example_duration_1
        "duration/1.cxx"
)

add_example(
    cxxet_example_complete_1
        "complete/1.cxx"
)

add_example(
    cxxet_example_local_file_sink_1
        "sink_diversion/1.cxx"
)

add_example(
    cxxet_example_local_file_sink_2
        "sink_diversion/2.cxx"
)

add_example(
    cxxet_example_local_file_sink_3
        "sink_diversion/3.cxx"
)

add_example(
    cxxet_example_local_file_sink_4
        "sink_diversion/4.cxx"
)

add_example(
    cxxet_test_init
        "for_tests/only_init.cxx"
)

add_example(
    cxxet_test_empty_file_1
        "for_tests/empty_file_1.cxx"
)

add_example(
    cxxet_test_empty_file_2
        "for_tests/empty_file_2.cxx"
)

add_example(
    cxxet_test_split_files
        "for_tests/split_files.cxx"
)

add_example(
    cxxet_test_reading_env
        "for_tests/reading_env.cxx"
)

add_example(
    cxxet_test_failed_init_1
        "for_tests/failed_init_1.cxx"
)

add_example(
    cxxet_test_failed_init_2
        "for_tests/failed_init_2.cxx"
)
