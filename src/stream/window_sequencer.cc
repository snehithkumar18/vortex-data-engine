#include "vde/stream/window_sequencer.h"
#include <algorithm>

namespace vde {

WindowSequencer::WindowSequencer(size_t window_size)
    : window_size_(window_size) {}

void WindowSequencer::clear() {
    window_.clear();
    expected_seq_ = 0;
}

bool WindowSequencer::add_fragment(Fragment frag) {
    if (window_.size() >= window_size_) return false;
    window_.push_back(std::move(frag));
    std::sort(window_.begin(), window_.end(), [](const Fragment& a, const Fragment& b) {
        return a.sequence_num < b.sequence_num;
    });
    return true;
}

bool WindowSequencer::has_next() const {
    if (window_.empty()) return false;
    return window_.front().sequence_num == expected_seq_;
}

Fragment WindowSequencer::pop_next() {
    if (!has_next()) return Fragment();
    Fragment frag = std::move(window_.front());
    window_.erase(window_.begin());
    expected_seq_++;
    return frag;
}

} // namespace vde
