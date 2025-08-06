add_executable(memoryqueue memoryqueue.cpp)

target_link_libraries(memoryqueue PRIVATE cpp_experiments_options cpp_experiments_warnings)

target_include_directories(memoryqueue ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(memoryqueue PUBLIC cxx_std_20)

set_target_properties(
  memoryqueue
  PROPERTIES VERSION ${PROJECT_VERSION})
