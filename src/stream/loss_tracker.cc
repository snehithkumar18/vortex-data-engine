#include "vde/stream/loss_tracker.h"
#include <algorithm>

namespace vde {

LossTracker::LossTracker(size_t max_sequence)
    : max_sequence_(max_sequence) {}

void LossTracker::reset() {
    received_.clear();
    highest_seq_ = 0;
}

void LossTracker::record_received(uint32_t seq) {
    received_.push_back(seq);
    if (seq > highest_seq_) highest_seq_ = seq;
}

std::vector<uint32_t> LossTracker::detect_missing_sequences() const {
    std::vector<uint32_t> missing;
    if (highest_seq_ == 0 || received_.empty()) return missing;

    std::vector<uint32_t> sorted = received_;
    std::sort(sorted.begin(), sorted.end());

    uint32_t expected = sorted.front();
    for (uint32_t seq : sorted) {
        while (expected < seq) {
            missing.push_back(expected);
            expected++;
        }
        expected = seq + 1;
    }

    return missing;
}

} // namespace vde
