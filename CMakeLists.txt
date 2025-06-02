cmake_minimum_required(VERSION 3.16)

project(
    runtime_simple_markers
    DESCRIPTION "TODO"
    LANGUAGES CXX # TODO later maybe even `C`?!
)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON" CACHE BOOL "Export compile commands" FORCE)

#set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# options:
set(RSM_BUILD_EXAMPLES "OFF" CACHE BOOL "Build all examples")
set(RSM_BUILD_SHARED "OFF" CACHE BOOL "Build as a shared library")
set(RSM_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/implicit" CACHE PATH "Folder with compilation results")
set(RSM_WITH_ASAN "OFF" CACHE BOOL "Build with address, leak and undefined sanitizers")
set(RSM_WITH_TSAN "OFF" CACHE BOOL "Build with thread sanitizer") # not using with `ubsan` ... https://github.com/google/sanitizers/issues/1218#issuecomment-608545162

# implied by options:
set(RSM_LIB_KIND "STATIC")
if (RSM_BUILD_SHARED)
    set(RSM_LIB_KIND "SHARED")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${RSM_OUTPUT_DIRECTORY}" CACHE PATH "Folder with compiled archives" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${RSM_OUTPUT_DIRECTORY}" CACHE PATH "Folder with compiled libraries" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RSM_OUTPUT_DIRECTORY}" CACHE PATH "Folder with compiled executables" FORCE)

if (RSM_WITH_ASAN AND RSM_WITH_TSAN)
    message(FATAL_ERROR "Cannot build with address and thread sanitizers at the same time")
endif()

if (RSM_WITH_ASAN OR RSM_WITH_TSAN)
    set(RSM_BUILD_TESTS "ON")
else()
    set(RSM_BUILD_TESTS "OFF" CACHE BOOL "Build tests")
endif()

include(FetchContent)

function(add_unix_compiler_flags target)
    # TODO
    # * get inspired by `doctest`: <https://github.com/doctest/doctest/blob/master/scripts/cmake/common.cmake#L84>
    # * refactor:
    set(RSM_UNIX_COMPILER_FLAGS
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
            RSM_COMPILER_FLAGS
                ${RSM_UNIX_COMPILER_FLAGS}
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(
            RSM_COMPILER_FLAGS
                ${RSM_UNIX_COMPILER_FLAGS}
                -Weffc++
        )
    endif()

    target_compile_options(
        ${target}
            PUBLIC
            ${RSM_COMPILER_FLAGS}
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

    if (RSM_WITH_ASAN)
        add_sanitizer(${target} address)
        add_sanitizer(${target} leak)
        add_sanitizer(${target} undefined)
    endif()
    if (RSM_WITH_TSAN)
        add_sanitizer(${target} thread)
    endif()

    get_target_property(INPUT_TARGET_TYPE ${target} TYPE)
    if (INPUT_TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        target_compile_options(
            ${target}
                PRIVATE
                -fvisibility=hidden
        )
        # https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html#index-fPIC
        target_compile_options(
            ${target}
                PUBLIC
                -fPIC
        )
    else()
        if (RSM_WITH_ASAN OR RSM_WITH_TSAN)
            target_compile_options(
                ${target}
                    PUBLIC
                    -fPIE
            )
            target_link_options(
                ${target}
                    PUBLIC
                    -pie
            )
        endif()
    endif()
endfunction()

# core library; source files:
# https://cmake.org/cmake/help/latest/command/file.html#filesystem
file(
    GLOB_RECURSE
    RSM_LIB_SOURCES
        CONFIGURE_DEPENDS
        "src/*.cpp"
)
add_library(
    rsm
        ${RSM_LIB_KIND}
        ${RSM_LIB_SOURCES}
)
add_unix_compiler_flags(rsm)
add_sanitizers(rsm)
if (RSM_BUILD_SHARED)
    set_target_properties( # seems useful -> TODO polish it later
        rsm
            PROPERTIES
            VERSION 0.1.0 # https://cmake.org/cmake/help/latest/prop_tgt/VERSION.html
            SOVERSION 0 # https://cmake.org/cmake/help/latest/prop_tgt/SOVERSION.html
    )
endif()
target_include_directories(
    rsm
        PUBLIC
        "include/public"
)
target_include_directories(
    rsm
        PRIVATE
        "include/private"
)

# tests, etc.:
if (RSM_BUILD_TESTS)
    add_subdirectory(tests/infra)

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11
    )
    FetchContent_MakeAvailable(doctest)

    add_executable(
        rsm_unit_tests
            tests/unit/doctest_main.cpp
            ${RSM_LIB_SOURCES}
    )
    target_include_directories(
        rsm_unit_tests
            PRIVATE
            "include/public"
            "include/private"
    )
    target_link_libraries(
        rsm_unit_tests
            PRIVATE
            doctest::doctest
    )
    target_compile_definitions(
        rsm_unit_tests
            PRIVATE
            DOCTEST_CONFIG_SUPER_FAST_ASSERTS
            DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS
            RSM_WITH_UNIT_TESTS
    )
    add_unix_compiler_flags(rsm_unit_tests)
    add_sanitizers(rsm_unit_tests)
endif()

if (RSM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
