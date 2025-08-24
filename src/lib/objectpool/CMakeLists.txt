# Header-only library
add_library(objectpool_lib INTERFACE)
add_library(cpp_experiments::objectpool ALIAS objectpool_lib)

target_include_directories(objectpool_lib INTERFACE
                            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                        )
target_compile_features(objectpool_lib INTERFACE cxx_std_20)