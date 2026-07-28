#include "vde/common/lru_cache.h"

namespace vde {

LruCache::LruCache(size_t capacity)
    : capacity_(capacity == 0 ? 1 : capacity) {}

void LruCache::put(uint32_t key, uint32_t value) {
    auto it = items_map_.find(key);
    if (it != items_map_.end()) {
        items_list_.erase(it->second);
        items_map_.erase(it);
    } else if (items_map_.size() >= capacity_) {
        auto last = items_list_.back();
        items_map_.erase(last.first);
        items_list_.pop_back();
    }

    items_list_.push_front({key, value});
    items_map_[key] = items_list_.begin();
}

bool LruCache::get(uint32_t key, uint32_t* out_value) {
    auto it = items_map_.find(key);
    if (it == items_map_.end()) return false;

    items_list_.splice(items_list_.begin(), items_list_, it->second);
    if (out_value) *out_value = it->second->second;
    return true;
}

bool LruCache::remove(uint32_t key) {
    auto it = items_map_.find(key);
    if (it == items_map_.end()) return false;

    items_list_.erase(it->second);
    items_map_.erase(it);
    return true;
}

} // namespace vde
