#include "vde/stream/reassembly_cache.h"
#include <algorithm>

namespace vde {

ReassemblyCache::ReassemblyCache(size_t max_sessions)
    : max_sessions_(max_sessions == 0 ? 16 : max_sessions) {}

Session* ReassemblyCache::find(uint32_t stream_id) {
    for (auto& entry : entries_) {
        if (entry.stream_id == stream_id) {
            entry.last_access = ++access_counter_;
            return entry.session.get();
        }
    }
    return nullptr;
}

Session* ReassemblyCache::get_or_create(uint32_t stream_id) {
    Session* existing = find(stream_id);
    if (existing) return existing;

    if (is_full()) {
        evict_oldest();
    }

    CacheEntry entry;
    entry.stream_id = stream_id;
    entry.session = std::make_unique<Session>(stream_id);
    entry.last_access = ++access_counter_;


    entries_.push_back(std::move(entry));
    return entries_.back().session.get();
}

void ReassemblyCache::remove(uint32_t stream_id) {
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
            [stream_id](const CacheEntry& e) { return e.stream_id == stream_id; }),
        entries_.end()
    );
}

void ReassemblyCache::evict_oldest() {
    if (entries_.empty()) return;

    auto oldest_it = std::min_element(entries_.begin(), entries_.end(),
        [](const CacheEntry& a, const CacheEntry& b) {
            return a.last_access < b.last_access;
        });

    if (oldest_it != entries_.end()) {
        entries_.erase(oldest_it);
    }
}

}
