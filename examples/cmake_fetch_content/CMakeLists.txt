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

FetchContent_Declare(
    cxxet
    GIT_REPOSITORY https://github.com/Ruzovej/cxxet.git
    GIT_TAG        v0.1.0
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
