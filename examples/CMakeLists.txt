add_custom_target(
    cxxst_examples
)

function(add_example target_name source_file)
    add_executable(
        ${target_name}
            ${source_file}
    )

    target_link_libraries(
        ${target_name}
            PRIVATE
            cxxst
    )

    add_executable(
        ${target_name}_bare
            ${source_file}
    )

    target_link_libraries(
        ${target_name}_bare
            PRIVATE
            cxxst_bare
    )

    add_dependencies(
        cxxst_examples
            ${target_name}
            ${target_name}_bare
    )
endfunction()

add_example(
    cxxst_example_instant_1
        "instant/1.cpp"
)

add_example(
    cxxst_example_instant_2
        "instant/2.cpp"
)

add_example(
    cxxst_example_counter_1
        "counter/1.cpp"
)

add_example(
    cxxst_example_counter_2
        "counter/2.cpp"
)

add_example(
    cxxst_example_duration_1
        "duration/1.cpp"
)

add_example(
    cxxst_example_complete_1
        "complete/1.cpp"
)

add_example(
    cxxst_test_init
        "for_tests/only_init.cpp"
)

add_example(
    cxxst_test_empty_file
        "for_tests/empty_file.cpp"
)
