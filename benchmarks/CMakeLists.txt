find_package(benchmark REQUIRED)

add_executable(basic_benchmark
    bench_test.cxx
)
target_link_libraries(basic_benchmark
    PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        cxxet
)
add_unix_compiler_flags(basic_benchmark)
add_sanitizers(basic_benchmark)
