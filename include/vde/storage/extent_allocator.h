#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdint>

namespace vde {

static constexpr size_t kExtentSizePages = 64;

struct ExtentHeader {
    uint32_t extent_id;
    uint32_t start_page_id;
    uint32_t pages_used;
    bool is_full;
};

class ExtentAllocator {
public:
    explicit ExtentAllocator(size_t total_extents = 16);
    ~ExtentAllocator() = default;

    int allocate_extent();
    Status free_extent(uint32_t extent_id);

    uint32_t allocate_page_in_extent(uint32_t extent_id);

    size_t active_extents() const { return active_extents_; }
    size_t total_extents() const { return extents_.size(); }

private:
    std::vector<ExtentHeader> extents_;
    std::vector<bool> bitmap_;
    size_t active_extents_ = 0;
};

}
