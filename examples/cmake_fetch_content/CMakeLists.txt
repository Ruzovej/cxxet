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

# e.g.:
# $ rm -rf build .cache
# $ time cmake -S . -B build -DCMAKE_BUILD_TYPE=Release "-DCXXET_ROOT_DIR=/home/lukas/tmp/cxxet"
# ...
# $ time cmake --build build -j 20
# ...
FetchContent_Declare(
    cxxet
    GIT_REPOSITORY "file://${CXXET_ROOT_DIR}"
    GIT_TAG v0.1.0 # TODO use current state of the repo ... at least latest commit on checked out branch
)
FetchContent_MakeAvailable(cxxet)

add_executable(
    cxxet_fetch_content_example
        1.cxx
)
target_link_libraries(
    cxxet_fetch_content_example
        PRIVATE
        cxxet
)
