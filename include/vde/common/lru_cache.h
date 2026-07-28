#pragma once

#include "vde/common/types.h"
#include <unordered_map>
#include <list>
#include <cstdint>

namespace vde {

class LruCache {
public:
    explicit LruCache(size_t capacity = 100);
    ~LruCache() = default;

    void put(uint32_t key, uint32_t value);
    bool get(uint32_t key, uint32_t* out_value);
    bool remove(uint32_t key);

    size_t size() const { return items_map_.size(); }
    size_t capacity() const { return capacity_; }

private:
    size_t capacity_;
    using KeyVal = std::pair<uint32_t, uint32_t>;
    std::list<KeyVal> items_list_;
    std::unordered_map<uint32_t, std::list<KeyVal>::iterator> items_map_;
};

} // namespace vde
