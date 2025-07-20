cmake_minimum_required(VERSION 3.16)

project(
    cxxet
    DESCRIPTION "C++ easy tracing"
    LANGUAGES CXX # TODO later maybe even `C`?!
)

# Check if this is being used as a subdirectory via FetchContent or add_subdirectory
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CXXET_CMAKE_FETCHED "ON")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON" CACHE BOOL "Export compile commands" FORCE)

#set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# options:
set(CXXET_BUILD_EXAMPLES "OFF" CACHE BOOL "Build all examples")
set(CXXET_BUILD_SHARED "OFF" CACHE BOOL "Build as a shared library")
set(CXXET_OUTPUT_DIRECTORY "" CACHE PATH "Folder with compilation results")
set(CXXET_WITH_ASAN "OFF" CACHE BOOL "Build with address, leak and undefined sanitizers")
set(CXXET_WITH_TSAN "OFF" CACHE BOOL "Build with thread sanitizer") # not using with `ubsan` ... https://github.com/google/sanitizers/issues/1218#issuecomment-608545162

# implied by options:
set(CXXET_LIB_KIND "STATIC")
if (CXXET_BUILD_SHARED)
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

if (CXXET_WITH_ASAN OR CXXET_WITH_TSAN)
    set(CXXET_BUILD_TESTS "ON")
else()
    set(CXXET_BUILD_TESTS "OFF" CACHE BOOL "Build tests")
endif()

include(FetchContent)

function(add_unix_compiler_flags target)
    # TODO
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
        -Wno-error=deprecated-declarations # TODO remove this or not?!
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
    # TODO refactor:
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
        "src/*.cxx"
)

# tests, etc.:
if (CXXET_BUILD_TESTS AND NOT CXXET_CMAKE_FETCHED)
    add_subdirectory(tests/infra)

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11
    )
    FetchContent_MakeAvailable(doctest)

    add_executable(
        cxxet_unit_tests
            tests/unit/doctest_main.cxx
            ${CXXET_LIB_SOURCES}
    )
    target_include_directories(
        cxxet_unit_tests
            PRIVATE
            "include/public"
            "include/private"
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
if (CXXET_BUILD_SHARED)
    add_shared_lib_flags(cxxet)
    set_target_properties( # seems useful -> TODO polish it later
        cxxet
            PROPERTIES
            VERSION 0.1.0 # https://cmake.org/cmake/help/latest/prop_tgt/VERSION.html
            SOVERSION 0 # https://cmake.org/cmake/help/latest/prop_tgt/SOVERSION.html
    )
endif()
target_include_directories(
    cxxet
        PUBLIC
        "include/all"
        "include/public"
)
target_include_directories(
    cxxet
        PRIVATE
        "include/private"
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
        "include/all"
)
add_library(
    cxxet::bare
        ALIAS
        cxxet_bare
)

if (CXXET_BUILD_EXAMPLES AND NOT CXXET_CMAKE_FETCHED)
    add_subdirectory(examples)
endif()
