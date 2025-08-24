add_library(objectpool_lib objectpool.cpp)
add_library(cpp_experiments::objectpool ALIAS objectpool_lib)

target_link_libraries(objectpool_lib PRIVATE cpp_experiments_options cpp_experiments_warnings)
target_include_directories(objectpool_lib ${WARNING_GUARD} PUBLIC
                            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                        )
target_compile_features(objectpool_lib PUBLIC cxx_std_20)

set_target_properties(
    objectpool_lib
    PROPERTIES VERSION ${PROJECT_VERSION}
)