FetchContent_Declare(
    benchmark
    GIT_REPOSITORY  https://github.com/google/benchmark.git
    GIT_TAG         v1.9.4
    GIT_SHALLOW     TRUE
)
option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)
option(BENCHMARK_ENABLE_EXCEPTIONS "Enable the use of exceptions in the benchmark library." ON)
option(BENCHMARK_ENABLE_LTO "Enable link time optimisation of the benchmark library." OFF)
option(BENCHMARK_USE_LIBCXX "Build and test using libc++ as the standard library." OFF)
option(BENCHMARK_ENABLE_WERROR "Build Release candidates with -Werror." OFF)
option(BENCHMARK_FORCE_WERROR "Build Release candidates with -Werror regardless of compiler issues." OFF)
option(BENCHMARK_ENABLE_INSTALL "Enable installation of benchmark. (Projects embedding benchmark may want to turn this OFF.)" OFF)
option(BENCHMARK_ENABLE_DOXYGEN "Build documentation with Doxygen." OFF)
option(BENCHMARK_INSTALL_DOCS "Enable installation of documentation." OFF)
option(BENCHMARK_INSTALL_TOOLS "Enable installation of tools." OFF)
option(BENCHMARK_ENABLE_GTEST_TESTS "Enable building the unit tests which depend on gtest" OFF)
option(BENCHMARK_USE_BUNDLED_GTEST "Use bundled GoogleTest. If disabled, the find_package(GTest) will be used." OFF)
option(BENCHMARK_ENABLE_LIBPFM "Enable performance counters provided by libpfm" OFF)
option(BENCHMARK_ENABLE_ASSEMBLY_TESTS "Enable building and running the assembly tests" OFF)
FetchContent_MakeAvailable(benchmark)

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
