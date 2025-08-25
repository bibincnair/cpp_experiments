cmake_minimum_required(VERSION 3.15...3.23)
project(CmakeConfigPackagebenchmarks LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  GoogleBenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.9.4
FetchContent_MakeAvailable(GoogleBenchmark)

# Add benchmark for objectpool.
add_executable(pool_benchmarks benchmarks/bench_objectpool.cpp)
target_link_libraries(pool_benchmarks PRIVATE benchmark::benchmark_main)