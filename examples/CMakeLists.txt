add_custom_target(
    cxxet_examples
)

function(add_example target_name source_file)
    add_executable(
        "${target_name}"
            ${source_file}
    )
    target_link_libraries(
        "${target_name}"
            PRIVATE
            cxxet
    )
    add_unix_compiler_flags("${target_name}")
    add_sanitizers("${target_name}")

    add_executable(
        "${target_name}_bare"
            ${source_file}
    )
    target_link_libraries(
        "${target_name}_bare"
            PRIVATE
            cxxet_bare
    )
    add_unix_compiler_flags("${target_name}_bare")
    add_sanitizers("${target_name}_bare")

    add_dependencies(
        cxxet_examples
            "${target_name}"
            "${target_name}_bare"
    )
endfunction()

add_subdirectory(for_tests)
add_subdirectory(usage)
