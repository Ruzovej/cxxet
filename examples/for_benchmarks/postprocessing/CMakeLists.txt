add_executable(
    cxxet_large_bench_postprocess
        main.cxx
)
target_link_libraries(
    cxxet_large_bench_postprocess
        PUBLIC
            nlohmann_json::nlohmann_json
)
add_unix_compiler_flags(cxxet_large_bench_postprocess)
add_sanitizers(cxxet_large_bench_postprocess)

add_dependencies(
    cxxet_large_benchmarks
        cxxet_large_bench_postprocess
)
