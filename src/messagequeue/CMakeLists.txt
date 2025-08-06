# Create a library for messagequeue
add_library(messagequeue_lib message_queue.cpp)

# Add an alias for easier linking
add_library(cpp_experiments::messagequeue ALIAS messagequeue_lib)

target_link_libraries(messagequeue_lib PRIVATE cpp_experiments_options cpp_experiments_warnings)

target_include_directories(messagequeue_lib ${WARNING_GUARD} PUBLIC 
                          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                          $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(messagequeue_lib PUBLIC cxx_std_20)

set_target_properties(
  messagequeue_lib
  PROPERTIES VERSION ${PROJECT_VERSION})

# Add executable linking main.cpp to messagequeue_lib
add_executable(messagequeue_main main.cpp)

target_link_libraries(messagequeue_main PRIVATE 
                      messagequeue_lib 
                      cpp_experiments_options 
                      cpp_experiments_warnings)

target_compile_features(messagequeue_main PUBLIC cxx_std_20)

set_target_properties(
  messagequeue_main
  PROPERTIES VERSION ${PROJECT_VERSION})
