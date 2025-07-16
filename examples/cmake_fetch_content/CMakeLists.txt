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
# $ time cmake --build build -j 20
# ...
FetchContent_Declare(
    cxxet
    GIT_REPOSITORY "${CXXET_GIT_REPOSITORY}"
    GIT_TAG "${CXXET_GIT_TAG}"
)
FetchContent_MakeAvailable(cxxet)

add_executable(
    cxxet_fetch_content_example
        1.cxx
)
target_link_libraries(
    cxxet_fetch_content_example
        PRIVATE
        cxxet::trace
)

add_executable(
    cxxet_fetch_content_example_bare
        1.cxx
)
target_link_libraries(
    cxxet_fetch_content_example_bare
        PRIVATE
        cxxet::bare
)
