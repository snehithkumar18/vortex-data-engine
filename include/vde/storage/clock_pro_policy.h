#pragma once

#include "vde/common/types.h"
#include <vector>
#include <unordered_map>

namespace vde {

enum class PageState {
    Cold,
    Hot,
    NonResident
};

struct ClockProPageEntry {
    uint32_t page_id;
    PageState state;
    bool referenced;
};

class ClockProPolicy {
public:
    explicit ClockProPolicy(size_t capacity = 16);
    ~ClockProPolicy() = default;

    void access_page(uint32_t page_id);
    uint32_t evict_page();

    size_t hot_count() const { return hot_count_; }
    size_t cold_count() const { return cold_count_; }

private:
    size_t capacity_;
    size_t hot_capacity_;
    size_t hot_count_ = 0;
    size_t cold_count_ = 0;

    std::vector<ClockProPageEntry> hand_cold_;
    std::unordered_map<uint32_t, size_t> page_map_;
};

} // namespace vde
