#include <vector>
#include <cstdint>
#include <string>

struct Message {
    uint64_t timestamp_ns;
    std::string topic;
    std::vector<uint8_t> data;
};
