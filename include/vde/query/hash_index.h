#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdint>

namespace vde {

class HashIndex {
public:
    explicit HashIndex(size_t capacity = 1024);
    ~HashIndex() = default;

    void insert(uint32_t key, size_t record_index);
    std::vector<size_t> lookup(uint32_t key) const;

    size_t size() const { return size_; }

private:
    struct HashEntry {
        uint32_t key;
        size_t record_index;
        bool occupied = false;
    };

    void resize_if_needed();

    std::vector<HashEntry> buckets_;
    size_t size_ = 0;
};

} // namespace vde
