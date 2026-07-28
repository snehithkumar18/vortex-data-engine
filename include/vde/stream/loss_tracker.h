#pragma once

#include "vde/stream/fragment.h"
#include <vector>
#include <cstdint>

namespace vde {

class LossTracker {
public:
    explicit LossTracker(size_t max_sequence = 10000);

    void record_received(uint32_t seq);
    std::vector<uint32_t> detect_missing_sequences() const;

    size_t received_count() const { return received_.size(); }
    void reset();

private:
    size_t max_sequence_;
    uint32_t highest_seq_ = 0;
    std::vector<uint32_t> received_;
};

}
