cmake_minimum_required(VERSION 3.16)

project(
    runtime_simple_markers
    DESCRIPTION "TODO"
    LANGUAGES CXX # TODO later maybe even `C`?!
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# options:
set(RSM_BUILD_SHARED "OFF" CACHE BOOL "Build as a shared library")
set(RSM_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/implicit" CACHE PATH "Folder with compilation results")
set(RSM_WITH_ASAN "OFF" CACHE BOOL "Build with address, leak and undefined sanitizers")
set(RSM_WITH_TSAN "OFF" CACHE BOOL "Build with thread and undefined sanitizers")

# implied by options:
set(RSM_LIB_KIND "STATIC")
if (RSM_BUILD_SHARED)
    set(RSM_LIB_KIND "SHARED") # TODO fix this
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

# source files:
# https://cmake.org/cmake/help/latest/command/file.html#filesystem
file(
    GLOB_RECURSE
    rsm_sources
        CONFIGURE_DEPENDS
        "src/*.cpp"
)

# TODO refactor:
function(add_sanitizer target sanitizer)
    target_compile_options(
        ${target}
            PUBLIC
            -fsanitize=${sanitizer}
            -fsanitize-recover=all
            -fno-omit-frame-pointer
            -fPIE
            -pie
    )
    target_link_options(
        ${target}
            PUBLIC
            -fsanitize=${sanitizer}
            -fsanitize-recover=all
            -fno-omit-frame-pointer
            -fPIE
            -pie
    )
endfunction()

# core library:
add_library(
    rsm
        ${RSM_LIB_KIND}
        ${rsm_sources}
)
target_include_directories(
    rsm
        PUBLIC
        "include"
)
if (RSM_WITH_ASAN)
    add_sanitizer(rsm address)
    add_sanitizer(rsm leak)
    add_sanitizer(rsm undefined)
endif()
if (RSM_WITH_TSAN)
    add_sanitizer(rsm thread)
    add_sanitizer(rsm undefined)
endif()
# TODO refactor:
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
    rsm
        PUBLIC
        ${RSM_COMPILER_FLAGS}
)

# tests, etc.:
if (RSM_BUILD_TESTS)
    add_subdirectory(tests/dummy_app)
    add_subdirectory(tests/unit)
endif()
