#include "apps/parallel_word_counter.h"
#include <iostream>
#include <sstream>

namespace apps {
ParallelWordCounter::ParallelWordCounter(size_t num_threads, std::string filepath)
  : m_num_threads(num_threads), m_thread_pool(num_threads)
{

  std::ifstream infile(filepath);
  infile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

  try {
    std::stringstream buffer;
    buffer << infile.rdbuf();
    m_file_content = std::move(buffer.str());
  } catch (const std::ios_base::failure &e) {
    throw std::runtime_error("Failed to read file" + filepath + "-" + e.what());
  }
}

void ParallelWordCounter::Partition(std::vector<std::string_view> &partition)
{
  partition.clear();

  size_t content_length = m_file_content.length();
  if (content_length == 0) { return; }
  size_t chunk_size = content_length / m_num_threads;
  partition.reserve(m_num_threads);

  size_t current_pos = 0;
  for (size_t i = 0; i < m_num_threads; i++) {

    if (i == m_num_threads - 1) {
      partition.emplace_back(m_file_content.data() + current_pos, content_length - current_pos);
      return;
    }
    size_t end_pos = std::min(content_length, current_pos + chunk_size);

    auto FindNextWord = [this](size_t pos) -> size_t {
      while (pos < m_file_content.length() && m_file_content[pos] != ' ' && m_file_content[pos] != '\n') { pos++; }
      return pos;
    };

    end_pos = FindNextWord(end_pos);
    partition.emplace_back(m_file_content.data() + current_pos, end_pos - current_pos);
    current_pos = end_pos;
    while (
      current_pos < content_length && (m_file_content[current_pos] == ' ' || m_file_content[current_pos] == '\n')) {
      current_pos++;
    }
    if (current_pos > content_length) { break; }
  }
}

auto ParallelWordCounter::GetTotalWordCount(bool print) -> std::map<std::string, int, std::less<>>
{
  std::vector<std::string_view> data_per_thread_partition;
  Partition(data_per_thread_partition);

  using task_result = std::map<std::string, int, std::less<>>;
  std::vector<std::future<task_result>> task_futures;


  auto SkipSeparators = [](const char* ptr, const char* end) -> const char* {
    while (ptr < end && (*ptr == ' ' || *ptr == '\n' || *ptr == '\t' || *ptr == '\r')) ++ptr;
    return ptr;
  };
  auto FindNextWord = [](const char* ptr, const char* end) -> const char* {
    while (ptr < end && (*ptr != ' ' && *ptr != '\n' && *ptr != '\t' && *ptr != '\r')) ++ptr;
    return ptr;
  };

  for (auto chunk : data_per_thread_partition) {
    auto word_counting_task = [chunk, SkipSeparators, FindNextWord]() -> task_result {
      task_result per_thread_word_count;
      const char* ptr = chunk.data();
      const char* end = ptr + chunk.size();
      std::string word;
      word.reserve(45); // Assuming english and largest word size of 45.
      while(ptr < end){
        ptr= SkipSeparators(ptr, end);
        const char* start = ptr;
        ptr = FindNextWord(ptr, end);
        if(start < ptr){
          word.assign(start, static_cast<size_t>(ptr - start));
          auto iter = per_thread_word_count.find(word);
          if(iter != per_thread_word_count.end()){
            iter->second++;
          }
          else{
            per_thread_word_count.emplace(std::string(word), 1);
          }
          
        }
      }
      return per_thread_word_count;
    };
    task_futures.push_back(m_thread_pool.enqueue(std::move(word_counting_task)));
  }

  task_result total_word_count;
  for(auto& f: task_futures){
    auto partial = f.get();
    for(auto& kv : partial){
      total_word_count[kv.first] += kv.second;
    }
  }

  if(print){
    for(auto& kv: total_word_count){
      std::cout << kv.first << ": " << kv.second << '\n';
    }
  }

  return total_word_count;
}
}// namespace apps


int main(int argc, char* argv[]){

  auto total_hardware_threads = std::thread::hardware_concurrency();
  auto parser_thread_count = std::min(total_hardware_threads/4, 2U);
  std::string filename("sample_text.txt");
  apps::ParallelWordCounter word_counter(static_cast<size_t>(parser_thread_count), filename);
  auto output = word_counter.GetTotalWordCount(true);
}