cmake_minimum_required(VERSION 3.16)

project(
    cxxet_fetch_content_example
    DESCRIPTION "testing this feature"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

if (NOT DEFINED CXXET_ROOT_DIR)
    set(CXXET_GIT_REPOSITORY "git@github.com:Ruzovej/cxxet.git")
else()
    set(CXXET_GIT_REPOSITORY "file://${CXXET_ROOT_DIR}")
endif()

if (NOT DEFINED CXXET_TAG)
    set(CXXET_GIT_TAG "main")
else()
    set(CXXET_GIT_TAG "${CXXET_TAG}")
endif()

# e.g. using repository from github & branch `main` (see defaults above):
# $ rm -rf build .cache build
# $ time cmake -S . -B build -DCMAKE_BUILD_TYPE=Release 
# ...
# $ time cmake --build build -j "$(nproc)"
# ...
set(CXXET_LIB_BUILD_AS_SHARED "ON" CACHE BOOL "Force building the fetched library as an '*.so' so it correctly links with the below defined shared lib." FORCE)
FetchContent_Declare(
    cxxet
    GIT_REPOSITORY  "${CXXET_GIT_REPOSITORY}"
    GIT_TAG         "${CXXET_GIT_TAG}"
    GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(cxxet)

# TODO (https://github.com/Ruzovej/cxxet/issues/12) declare only one target instead of two (using `cxxet::trace` vs. `cxxet::bare`), and decide which version to use based on a variable

add_library(
    cxxet_fetch_content_shared_lib_example_foo
        SHARED
        shared_lib_foo.cxx
)
target_link_libraries(
    cxxet_fetch_content_shared_lib_example_foo
        PUBLIC
        cxxet::trace
)
target_compile_options(
    cxxet_fetch_content_shared_lib_example_foo
        PRIVATE
        -fvisibility=hidden
        -fPIC
)

add_executable(
    cxxet_fetch_content_indirect_usage_example
        indirect_usage.cxx
)
target_link_libraries(
    cxxet_fetch_content_indirect_usage_example
        PRIVATE
        cxxet_fetch_content_shared_lib_example_foo
)

add_library(
    cxxet_fetch_content_shared_lib_example_foo_bare
        SHARED
        shared_lib_foo.cxx
)
target_link_libraries(
    cxxet_fetch_content_shared_lib_example_foo_bare
        PUBLIC
        cxxet::bare
)
target_compile_options(
    cxxet_fetch_content_shared_lib_example_foo_bare
        PRIVATE
        -fvisibility=hidden
        -fPIC
)

add_executable(
    cxxet_fetch_content_indirect_usage_example_bare
        indirect_usage.cxx
)
target_link_libraries(
    cxxet_fetch_content_indirect_usage_example_bare
        PRIVATE
        cxxet_fetch_content_shared_lib_example_foo_bare
)
