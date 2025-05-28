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

    if (RSM_WITH_ASAN)
        add_sanitizer(${target_name} address)
        add_sanitizer(${target_name} leak)
        add_sanitizer(${target_name} undefined)
    endif()
    if (RSM_WITH_TSAN)
        add_sanitizer(${target_name} thread)
        add_sanitizer(${target_name} undefined)
    endif()
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
