#include "vde/storage/memory_chunk_pool.h"
#include <algorithm>

namespace vde {

MemoryChunkPool::MemoryChunkPool(size_t pool_size)
    : pool_size_(pool_size == 0 ? 4 : pool_size) {
    frames_.resize(pool_size_);
}

PayloadFrameBlock* MemoryChunkPool::fetch_page(uint32_t page_id) {
    auto it = page_table_.find(page_id);
    if (it != page_table_.end()) {
        size_t frame_id = it->second;
        frames_[frame_id].pin_count++;
        return &frames_[frame_id].page;
    }
    return nullptr;
}

PayloadFrameBlock* MemoryChunkPool::new_page(uint32_t* out_page_id) {
    size_t target_frame = frames_.size();
    for (size_t i = 0; i < frames_.size(); ++i) {
        if (frames_[i].pin_count == 0) {
            target_frame = i;
            break;
        }
    }

    if (target_frame == frames_.size()) return nullptr;

    uint32_t pid = next_page_id_++;
    if (out_page_id) *out_page_id = pid;

    if (frames_[target_frame].page_id != 0) {
        page_table_.erase(frames_[target_frame].page_id);
    }

    frames_[target_frame].page_id = pid;
    frames_[target_frame].pin_count = 1;
    frames_[target_frame].is_dirty = false;
    frames_[target_frame].page.init(pid);

    page_table_[pid] = target_frame;
    return &frames_[target_frame].page;
}

bool MemoryChunkPool::unpin_page(uint32_t page_id, bool is_dirty) {
    auto it = page_table_.find(page_id);
    if (it == page_table_.end()) return false;
    size_t frame_id = it->second;
    if (frames_[frame_id].pin_count <= 0) return false;

    frames_[frame_id].pin_count--;
    if (is_dirty) frames_[frame_id].is_dirty = true;
    return true;
}

bool MemoryChunkPool::flush_page(uint32_t page_id) {
    auto it = page_table_.find(page_id);
    if (it == page_table_.end()) return false;
    size_t frame_id = it->second;
    frames_[frame_id].is_dirty = false;
    return true;
}

}
