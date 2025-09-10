include(FetchContent)

cmake_policy(SET CMP0135 NEW) # file timestamps of extracted data are updated to "now"
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    #GIT_REPOSITORY  https://github.com/nlohmann/json.git
    #GIT_TAG         v3.12.0
    #GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(json)

file(
    GLOB_RECURSE
    CXXET_BENCH_COMMON_LIB_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/common/sources/*.cxx"
)
add_library(
    cxxet_bench_common
        STATIC
            ${CXXET_BENCH_COMMON_LIB_SOURCES}
)
target_include_directories(
    cxxet_bench_common
        PUBLIC
            "${CMAKE_CURRENT_LIST_DIR}/common/headers"
)
target_link_libraries(
    cxxet_bench_common
        PRIVATE
            cxxet
        PUBLIC
            nlohmann_json::nlohmann_json
)
add_library(
    cxxet_bench_common_bare
        STATIC
            ${CXXET_BENCH_COMMON_LIB_SOURCES}
)
target_include_directories(
    cxxet_bench_common_bare
        PUBLIC
            "${CMAKE_CURRENT_LIST_DIR}/common/headers"
)
target_link_libraries(
    cxxet_bench_common_bare
        PRIVATE
            cxxet_bare
        PUBLIC
            nlohmann_json::nlohmann_json
)

function(add_bench_example target_name source_file)
    add_example("${target_name}" "${source_file}")

    target_link_libraries(
        "${target_name}"
            PRIVATE
                cxxet_bench_common
    )

    target_link_libraries(
        "${target_name}_bare"
            PRIVATE
                cxxet_bench_common_bare
    )
endfunction()

add_bench_example(
    cxxet_bench_init
        "bench_init.cxx"
)
