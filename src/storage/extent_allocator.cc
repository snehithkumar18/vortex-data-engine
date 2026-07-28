#include "vde/storage/extent_allocator.h"

namespace vde {

ExtentAllocator::ExtentAllocator(size_t total_extents)
    : extents_(total_extents), bitmap_(total_extents, false) {
    for (uint32_t i = 0; i < total_extents; ++i) {
        extents_[i].extent_id = i;
        extents_[i].start_page_id = static_cast<uint32_t>(i * kExtentSizePages);
        extents_[i].pages_used = 0;
        extents_[i].is_full = false;
    }
}

int ExtentAllocator::allocate_extent() {
    for (size_t i = 0; i < bitmap_.size(); ++i) {
        if (!bitmap_[i]) {
            bitmap_[i] = true;
            active_extents_++;
            return static_cast<int>(i);
        }
    }
    return -1;
}

Status ExtentAllocator::free_extent(uint32_t extent_id) {
    if (extent_id >= bitmap_.size() || !bitmap_[extent_id]) {
        return Status::InvalidArgument;
    }
    bitmap_[extent_id] = false;
    extents_[extent_id].pages_used = 0;
    extents_[extent_id].is_full = false;
    active_extents_--;
    return Status::Ok;
}

uint32_t ExtentAllocator::allocate_page_in_extent(uint32_t extent_id) {
    if (extent_id >= extents_.size() || extents_[extent_id].is_full) {
        return UINT32_MAX;
    }

    auto& ext = extents_[extent_id];
    uint32_t allocated_page = ext.start_page_id + ext.pages_used;
    ext.pages_used++;
    if (ext.pages_used >= kExtentSizePages) {
        ext.is_full = true;
    }
    return allocated_page;
}

}
