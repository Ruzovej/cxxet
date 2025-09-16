cmake_minimum_required(VERSION 3.16)

# https://cmake.org/cmake/help/latest/command/project.html
project(
    cxxet
    VERSION 0.1.0
    HOMEPAGE_URL "https://github.com/Ruzovej/cxxet"
    DESCRIPTION "C++ easy tracing"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON" CACHE BOOL "Export compile commands" FORCE)

# Check if this is being used as a subdirectory via FetchContent, add_subdirectory or similar ...
# Professional CMake 20th edition, 8.5. Recommended Practices:
#   - `if(PROJECT_IS_TOP_LEVEL)` # Requires CMake 3.21 or later
#   - prefer using `projectName_SOURCE_DIR` (determined by the last `project(...)` call) or `CMAKE_CURRENT_LIST_DIR`
if(NOT CMAKE_SOURCE_DIR STREQUAL cxxet_SOURCE_DIR)
    set(CXXET_CMAKE_FETCHED "ON")
else()
    set(CXXET_CMAKE_FETCHED "OFF")
endif()

# options:
set(CXXET_BUILD_BENCHMARKS "ON" CACHE BOOL "Build benchmarks")
set(CXXET_BUILD_EXAMPLES "ON" CACHE BOOL "Build all examples")
set(CXXET_BUILD_TESTS "ON" CACHE BOOL "Build tests")
set(CXXET_LIB_BUILD_AS_SHARED "OFF" CACHE BOOL "Build as a shared library")
set(CXXET_OUTPUT_DIRECTORY "" CACHE PATH "Folder with compilation results")
set(CXXET_WITH_ASAN "OFF" CACHE BOOL "Build with address, leak and undefined sanitizers")
set(CXXET_WITH_TSAN "OFF" CACHE BOOL "Build with thread sanitizer") # not using with `ubsan` ... https://github.com/google/sanitizers/issues/1218#issuecomment-608545162

# implied by options:
if (NOT CXXET_LIB_BUILD_AS_SHARED)
    set(CXXET_LIB_KIND "STATIC")
else()
    set(CXXET_LIB_KIND "SHARED")
endif()

if (NOT CXXET_CMAKE_FETCHED)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CXXET_OUTPUT_DIRECTORY}" CACHE PATH "Folder with compiled archives" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CXXET_OUTPUT_DIRECTORY}" CACHE PATH "Folder with compiled libraries" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CXXET_OUTPUT_DIRECTORY}" CACHE PATH "Folder with compiled executables" FORCE)
endif()

if (CXXET_WITH_ASAN AND CXXET_WITH_TSAN)
    message(FATAL_ERROR "Cannot build with address and thread sanitizers at the same time")
endif()

function(add_unix_compiler_flags target)
    # TODO (https://github.com/Ruzovej/cxxet/issues/12)
    # * get inspired by `doctest`: <https://github.com/doctest/doctest/blob/master/scripts/cmake/common.cmake#L84>
    # * refactor:
    set(CXXET_UNIX_COMPILER_FLAGS
        -Wall
        -Wextra
        -pedantic
        -Werror
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wmisleading-indentation
        -Wdouble-promotion
        -Wformat=2
        -Wnull-dereference
        -Wimplicit-fallthrough
        -Wno-error=deprecated-declarations # TODO (https://github.com/Ruzovej/cxxet/issues/12) remove this or not?!
    )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(
            CXXET_COMPILER_FLAGS
                ${CXXET_UNIX_COMPILER_FLAGS}
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(
            CXXET_COMPILER_FLAGS
                ${CXXET_UNIX_COMPILER_FLAGS}
                -Weffc++
        )
    endif()

    target_compile_options(
        ${target}
            PRIVATE
            ${CXXET_COMPILER_FLAGS}
    )
endfunction()

function(add_sanitizers target)
    # TODO (https://github.com/Ruzovej/cxxet/issues/12) refactor:
    function(add_sanitizer target sanitizer)
        target_compile_options(
            ${target}
                PUBLIC
                -fsanitize=${sanitizer}
                -fno-sanitize-recover=all
                -fno-omit-frame-pointer
        )
        target_link_options(
            ${target}
                PUBLIC
                -fsanitize=${sanitizer}
                -fno-sanitize-recover=all
                -fno-omit-frame-pointer
        )
    endfunction()

    if (CXXET_WITH_ASAN)
        add_sanitizer(${target} address)
        add_sanitizer(${target} leak)
        add_sanitizer(${target} undefined)
    endif()
    if (CXXET_WITH_TSAN)
        add_sanitizer(${target} thread)
    endif()

    get_target_property(INPUT_TARGET_TYPE ${target} TYPE)
    if (NOT INPUT_TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        if (CXXET_WITH_ASAN OR CXXET_WITH_TSAN)
            target_compile_options(
                ${target}
                    PRIVATE
                    -fPIE
            )
            target_link_options(
                ${target}
                    PRIVATE
                    -pie
            )
        endif()
    endif()
endfunction()

function(add_shared_lib_flags target)
    get_target_property(INPUT_TARGET_TYPE ${target} TYPE)
    if (INPUT_TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        target_compile_options(
            ${target}
                PRIVATE
                -fvisibility=hidden
                -fPIC # https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html#index-fPIC
        )
    else()
        message(FATAL_ERROR "'add_shared_lib_flags' called on a target that is not a shared library: ${target}")
    endif()
endfunction()

# core library; source files:
# https://cmake.org/cmake/help/latest/command/file.html#filesystem
file(
    GLOB_RECURSE
    CXXET_LIB_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/src/*.cxx"
)

include(FetchContent)

# first should be target that is desired to be picked by `clangd` for indexing the source code ... unfortunately, they define various macros to control what is being compiled beside regular source code (e.g. benchmarks, unit tests or nothing more), hence various parts of code will be ignored if not chosen correctly.
if (CXXET_BUILD_BENCHMARKS AND NOT CXXET_CMAKE_FETCHED)
    # TODO (https://github.com/Ruzovej/cxxet/issues/12) refactor this ... see the duplicated code for unit tests below ...
    cmake_policy(SET CMP0135 NEW) # file timestamps of extracted data are updated to "now"
    FetchContent_Declare(
        benchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.tar.gz
        #GIT_REPOSITORY  https://github.com/google/benchmark.git
        #GIT_TAG         v1.9.4
        #GIT_SHALLOW     TRUE
    )
    option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)
    option(BENCHMARK_ENABLE_EXCEPTIONS "Enable the use of exceptions in the benchmark library." ON)
    option(BENCHMARK_ENABLE_LTO "Enable link time optimisation of the benchmark library." OFF)
    option(BENCHMARK_USE_LIBCXX "Build and test using libc++ as the standard library." OFF)
    option(BENCHMARK_ENABLE_WERROR "Build Release candidates with -Werror." OFF)
    option(BENCHMARK_FORCE_WERROR "Build Release candidates with -Werror regardless of compiler issues." OFF)
    option(BENCHMARK_ENABLE_INSTALL "Enable installation of benchmark. (Projects embedding benchmark may want to turn this OFF.)" OFF)
    option(BENCHMARK_ENABLE_DOXYGEN "Build documentation with Doxygen." OFF)
    option(BENCHMARK_INSTALL_DOCS "Enable installation of documentation." OFF)
    option(BENCHMARK_INSTALL_TOOLS "Enable installation of tools." OFF)
    option(BENCHMARK_ENABLE_GTEST_TESTS "Enable building the unit tests which depend on gtest" OFF)
    option(BENCHMARK_USE_BUNDLED_GTEST "Use bundled GoogleTest. If disabled, the find_package(GTest) will be used." OFF)
    option(BENCHMARK_ENABLE_LIBPFM "Enable performance counters provided by libpfm" ON)
    option(BENCHMARK_ENABLE_ASSEMBLY_TESTS "Enable building and running the assembly tests" OFF)
    FetchContent_MakeAvailable(benchmark)

    add_executable(
        cxxet_benchmarks
            ${CXXET_LIB_SOURCES}
    )
    target_include_directories(
        cxxet_benchmarks
            PRIVATE
            "${cxxet_SOURCE_DIR}/include/public"
            "${cxxet_SOURCE_DIR}/include/private"
    )
    target_link_libraries(
        cxxet_benchmarks
            PRIVATE
                benchmark::benchmark
                benchmark::benchmark_main
    )
    target_compile_definitions(
        cxxet_benchmarks
            PRIVATE
            CXXET_ENABLE
            CXXET_WITH_BENCHMARKS
    )
    add_unix_compiler_flags(cxxet_benchmarks)
    add_sanitizers(cxxet_benchmarks)
endif()

if (CXXET_BUILD_TESTS AND NOT CXXET_CMAKE_FETCHED)
    add_subdirectory(tests/infra)

    cmake_policy(SET CMP0135 NEW) # file timestamps of extracted data are updated to "now"
    FetchContent_Declare(
        doctest
        URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.11.tar.gz
        #GIT_REPOSITORY  https://github.com/doctest/doctest.git
        #GIT_TAG         v2.4.11
        #GIT_SHALLOW     TRUE
    )
    FetchContent_MakeAvailable(doctest)

    add_executable(
        cxxet_unit_tests
            "${cxxet_SOURCE_DIR}/tests/unit/doctest_main.cxx"
            ${CXXET_LIB_SOURCES}
    )
    target_include_directories(
        cxxet_unit_tests
            PRIVATE
            "${cxxet_SOURCE_DIR}/include/public"
            "${cxxet_SOURCE_DIR}/include/private"
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
endif()

add_library(
    cxxet
        ${CXXET_LIB_KIND}
        ${CXXET_LIB_SOURCES}
)
add_unix_compiler_flags(cxxet)
add_sanitizers(cxxet)
if (CXXET_LIB_BUILD_AS_SHARED)
    add_shared_lib_flags(cxxet)
    set_target_properties( # seems useful -> TODO (https://github.com/Ruzovej/cxxet/issues/12) polish it later
        cxxet
            PROPERTIES
            VERSION "${cxxet_VERSION}" # https://cmake.org/cmake/help/latest/prop_tgt/VERSION.html
            SOVERSION "${cxxet_VERSION_MAJOR}" # https://cmake.org/cmake/help/latest/prop_tgt/SOVERSION.html
    )
endif()
target_include_directories(
    cxxet
        PUBLIC
        "${cxxet_SOURCE_DIR}/include/all"
        "${cxxet_SOURCE_DIR}/include/public"
)
target_include_directories(
    cxxet
        PRIVATE
        "${cxxet_SOURCE_DIR}/include/private"
)
target_compile_definitions(
    cxxet
        PUBLIC
        CXXET_ENABLE
)
add_library(
    cxxet::trace
        ALIAS
        cxxet
)

add_library(
    cxxet_bare
        INTERFACE
)
target_include_directories(
    cxxet_bare
        INTERFACE
        "${cxxet_SOURCE_DIR}/include/all"
)
add_library(
    cxxet::bare
        ALIAS
        cxxet_bare
)

if (CXXET_BUILD_EXAMPLES AND NOT CXXET_CMAKE_FETCHED)
    add_subdirectory(examples)
endif()
