#pragma once

#include "vde/storage/payload_frame_block.h"
#include <unordered_map>
#include <vector>
#include <list>

namespace vde {

class MemoryChunkPool {
public:
    explicit MemoryChunkPool(size_t pool_size = 16);
    ~MemoryChunkPool() = default;

    PayloadFrameBlock* fetch_page(uint32_t page_id);
    PayloadFrameBlock* new_page(uint32_t* out_page_id);
    bool unpin_page(uint32_t page_id, bool is_dirty);
    bool flush_page(uint32_t page_id);

    size_t pool_size() const { return pool_size_; }

private:
    struct Frame {
        uint32_t page_id = 0;
        int pin_count = 0;
        bool is_dirty = false;
        PayloadFrameBlock page;
    };

    size_t pool_size_;
    uint32_t next_page_id_ = 0;
    std::vector<Frame> frames_;
    std::unordered_map<uint32_t, size_t> page_table_;
    std::list<size_t> lru_list_;
};

}
