# Benchmarks subdirectory
# Note: This is included by the main project, not a standalone project

# Create the benchmark executable
add_executable(pool_benchmarks bench_objectpool.cpp)

# Link to the required libraries
target_link_libraries(pool_benchmarks PRIVATE
    benchmark::benchmark_main
    cpp_experiments::objectpool
    cpp_experiments_options
    cpp_experiments_warnings
)

target_compile_features(pool_benchmarks PRIVATE cxx_std_20)

# CTest integration (optional)
if(BUILD_TESTING)
    add_test(NAME ObjectPoolBenchmark COMMAND pool_benchmarks --benchmark_min_time=0.1)
endif()