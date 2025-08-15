# Application executables

# MessageQueue demo application
add_executable(messagequeue_demo messagequeue_main.cpp)
target_link_libraries(messagequeue_demo PRIVATE
                    cpp_experiments::messagequeue
                    cpp_experiments_options
                    cpp_experiments_warnings
                    )
target_compile_features(messagequeue_demo PUBLIC cxx_std_20)

# ThreadPool demo application  
add_executable(threadpool_demo threadpool_main.cpp)
target_link_libraries(threadpool_demo PRIVATE
                    cpp_experiments::threadpool
                    cpp_experiments_options
                    cpp_experiments_warnings
                    )
target_compile_features(threadpool_demo PUBLIC cxx_std_20)

# ThreadSafeQueue demo application
add_executable(threadsafequeue_demo threadsafequeue_main.cpp)
target_link_libraries(threadsafequeue_demo PRIVATE
                    cpp_experiments::threadsafequeue
                    cpp_experiments_options
                    cpp_experiments_warnings
                    )
target_compile_features(threadsafequeue_demo PUBLIC cxx_std_20)
