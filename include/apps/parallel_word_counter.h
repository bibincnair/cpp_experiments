#include "threadpool/threadpool.h"
#include <fstream>
#include <map>
#include <string>

namespace apps {

class ParallelWordCounter
{

private:
  size_t m_num_threads;
  std::string m_file_content;
  ThreadPool m_thread_pool;

  /**
   * @brief Partition the file over N chunks, where N is the number of threads
   * in threadpool.
   */
  void Partition(std::vector<std::string_view> &parition);

  /**
   * @brief Enqueues the partitioned data frames to be handled
   *
   */
  void Map(std::vector<std::string_view> &partition);

  /**
   * @brief Aggregates per thread map into a single std::map<std::string, int>
   *
   */
  void Reduce(std::map<std::string, int> &total_string_count);

public:

  ParallelWordCounter(size_t num_threads, std::string filepath);
  /**
   * @brief Get the individual word count for the input file.
   *
   * @param print : Pretty print the count if flag is set, default true.
   * @return std::map<std::string, int>
   */
  auto GetTotalWordCount(bool print = true) -> std::map<std::string, int, std::less<>>;
};
}// namespace apps