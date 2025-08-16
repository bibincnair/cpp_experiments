#include "apps/parallel_word_counter.h"

namespace apps {
ParallelWordCounter::ParallelWordCounter(size_t num_threads, std::string filepath) : m_thread_pool(num_threads) {}


void ParallelWordCounter::Map(std::map<std::string, int> &string_count) {}

auto ParallelWordCounter::Reduce() -> std::map<std::string, int> {}

void ParallelWordCounter::Partition(std::vector<std::string_view> &partition) {}
}// namespace apps