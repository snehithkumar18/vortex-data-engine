#pragma once

#include "vde/stream/session.h"
#include <memory>
#include <vector>

namespace vde {

class ReassemblyCache {
public:
    explicit ReassemblyCache(size_t max_sessions = 256);
    ~ReassemblyCache() = default;

    Session* get_or_create(uint32_t stream_id);
    Session* find(uint32_t stream_id);
    void remove(uint32_t stream_id);
    void evict_oldest();

    size_t size() const { return entries_.size(); }
    size_t capacity() const { return max_sessions_; }
    bool is_full() const { return entries_.size() >= max_sessions_; }

private:
    struct CacheEntry {
        uint32_t stream_id;
        std::unique_ptr<Session> session;
        uint64_t last_access;
    };

    std::vector<CacheEntry> entries_;
    size_t max_sessions_;
    uint64_t access_counter_ = 0;
};

} // namespace vde
