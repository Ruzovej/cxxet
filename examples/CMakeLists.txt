add_custom_target(
    rsm_examples
)

function(add_example target_name source_file)
    add_executable(
        ${target_name}
            ${source_file}
    )

    target_link_libraries(
        ${target_name}
            PRIVATE
            rsm
    )

    add_executable(
        ${target_name}_bare
            ${source_file}
    )

    target_link_libraries(
        ${target_name}_bare
            PRIVATE
            rsm_bare
    )

    add_dependencies(
        rsm_examples
            ${target_name}
            ${target_name}_bare
    )
endfunction()

add_example(
    rsm_example_instant_1
        "instant/1.cpp"
)

add_example(
    rsm_example_instant_2
        "instant/2.cpp"
)

add_example(
    rsm_example_counter_1
        "counter/1.cpp"
)

add_example(
    rsm_example_counter_2
        "counter/2.cpp"
)

add_example(
    rsm_example_duration_1
        "duration/1.cpp"
)

add_example(
    rsm_example_complete_1
        "complete/1.cpp"
)
