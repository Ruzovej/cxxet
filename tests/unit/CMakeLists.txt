include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.11
)
FetchContent_MakeAvailable(doctest)

file(
    GLOB_RECURSE
    rsm_unit_tests_sources
        CONFIGURE_DEPENDS
        "*.cpp"
)

add_executable(
    rsm_unit_tests
        ${rsm_unit_tests_sources}
)
target_link_libraries(
    rsm_unit_tests
        PRIVATE
        doctest::doctest
        rsm
)
target_compile_definitions(
    rsm_unit_tests
        PRIVATE
        DOCTEST_CONFIG_SUPER_FAST_ASSERTS
        DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
)
