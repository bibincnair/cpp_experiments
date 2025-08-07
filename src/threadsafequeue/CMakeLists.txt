add_library(threadsafequeue_lib thread_safe_queue.cpp)
add_library(cpp_experiments::threadsafequeue ALIAS threadsafequeue_lib)

target_link_libraries(threadsafequeue_lib PRIVATE cpp_experiments_options cpp_experiments_warnings)
target_include_directories(threadsafequeue_lib ${WARNING_GUARD} PUBLIC 
                          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                          $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(threadsafequeue_lib PUBLIC cxx_std_20)

set_target_properties(
  threadsafequeue_lib
  PROPERTIES VERSION ${PROJECT_VERSION})

# Add executable linking main.cpp to threadsafequeue_lib
add_executable(threadsafequeue_main main.cpp)

target_link_libraries(threadsafequeue_main PRIVATE 
                      threadsafequeue_lib 
                      cpp_experiments_options 
                      cpp_experiments_warnings
                      )

target_compile_features(threadsafequeue_main PUBLIC cxx_std_20)

set_target_properties(
  threadsafequeue_main
  PROPERTIES VERSION ${PROJECT_VERSION})
