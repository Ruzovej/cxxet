# TODO #156 refactor this ... see `benchmarks/CMakeLists.txt`

FetchContent_Declare(
    doctest
    GIT_REPOSITORY  https://github.com/doctest/doctest.git
    GIT_TAG         v2.4.11
    GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(doctest)

add_executable(
    cxxet_unit_tests
        "${CMAKE_SOURCE_DIR}/tests/unit/doctest_main.cxx"
        ${CXXET_LIB_SOURCES}
)
target_include_directories(
    cxxet_unit_tests
        PRIVATE
        "${CMAKE_SOURCE_DIR}/include/public"
        "${CMAKE_SOURCE_DIR}/include/private"
)
target_link_libraries(
    cxxet_unit_tests
        PRIVATE
        doctest::doctest
)
target_compile_definitions(
    cxxet_unit_tests
        PRIVATE
        CXXET_ENABLE
        CXXET_WITH_UNIT_TESTS
        DOCTEST_CONFIG_SUPER_FAST_ASSERTS
        DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
)
add_unix_compiler_flags(cxxet_unit_tests)
add_sanitizers(cxxet_unit_tests)
