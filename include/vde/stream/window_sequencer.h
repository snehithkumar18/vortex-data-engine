#pragma once

#include "vde/stream/fragment.h"
#include <vector>
#include <deque>

namespace vde {

class WindowSequencer {
public:
    explicit WindowSequencer(size_t window_size = 64);
    ~WindowSequencer() = default;

    bool add_fragment(Fragment frag);
    bool has_next() const;
    Fragment pop_next();

    size_t pending_count() const { return window_.size(); }
    void clear();

private:
    size_t window_size_;
    uint32_t expected_seq_ = 0;
    std::vector<Fragment> window_;
};

} // namespace vde
