#include "threadpool/threadpool.h"
#include <string>
#include <fstream>
#include <map>

namespace apps{

    class ParallelWordCounter{
        ParallelWordCounter(size_t num_threads, std::string filepath);
        
    private:
        std::string m_file_content;
        ThreadPool m_thread_pool;

        /**
         * @brief Partition the file over N chunks, where N is the number of threads
         * in threadpool.
         */
        void Partition(std::vector<std::string_view>& parition);

    public:
        /**
         * @brief Enqueues the partitioned data frames to be handled and returns a 
         * local std::map<std::string, int>
         * 
         */
        void Map(std::map<std::string, int>& string_count);

        /**
         * @brief Aggregates per thread map into a single std::map<std::string, int>
         * 
         * @return int 
         */
        auto Reduce() -> std::map<std::string, int>;
    };
}