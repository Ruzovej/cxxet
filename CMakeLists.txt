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

# TODO refactor:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin" CACHE PATH "Folder with compilation results" FORCE)
# + add various suffixes based upon build type ...

set(RSM_BUILD_TESTS "ON" CACHE BOOL "Build unit tests")

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
function(add_tsan target)
    add_sanitizer(${target} thread)
endfunction()

add_library(
    rsm
        STATIC
        ${rsm_sources}
)
target_include_directories(
    rsm
        PUBLIC
        "include"
)
add_sanitizer(rsm thread) # thread address undefined leak
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

if (RSM_BUILD_TESTS)
    add_subdirectory(tests/dummy_app)
    add_subdirectory(tests/unit)
endif()
