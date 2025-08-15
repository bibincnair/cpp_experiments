add_library(threadpool_lib threadpool.cpp)
add_library(cpp_experiments::threadpool ALIAS threadpool_lib)

target_link_libraries(threadpool_lib PRIVATE cpp_experiments_options cpp_experiments_warnings)
target_include_directories(threadpool_lib ${WARNING_GUARD} PUBLIC
                            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                        )
target_compile_features(threadpool_lib PUBLIC cxx_std_20)

set_target_properties(
    threadpool_lib
    PROPERTIES VERSION ${PROJECT_VERSION}
)