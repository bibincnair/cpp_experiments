# Create header-only library for threadsafequeue
add_library(threadsafequeue_lib INTERFACE)
add_library(cpp_experiments::threadsafequeue ALIAS threadsafequeue_lib)

target_include_directories(threadsafequeue_lib INTERFACE 
                          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                          $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(threadsafequeue_lib INTERFACE cxx_std_20)
