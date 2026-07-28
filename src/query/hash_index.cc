#include "vde/query/hash_index.h"
#include "vde/common/math_utils.h"

namespace vde {

HashIndex::HashIndex(size_t capacity)
    : buckets_(capacity == 0 ? 16 : capacity), size_(0) {}

void HashIndex::resize_if_needed() {
    if (size_ * 2 >= buckets_.size()) {
        size_t new_cap = buckets_.size() * 2;
        std::vector<HashEntry> old_buckets = std::move(buckets_);
        buckets_ = std::vector<HashEntry>(new_cap);
        size_ = 0;

        for (const auto& entry : old_buckets) {
            if (entry.occupied) {
                insert(entry.key, entry.record_index);
            }
        }
    }
}

void HashIndex::insert(uint32_t key, size_t record_index) {
    resize_if_needed();
    size_t idx = static_cast<size_t>(key) % buckets_.size();
    while (buckets_[idx].occupied) {
        idx = (idx + 1) % buckets_.size();
    }

    buckets_[idx].key = key;
    buckets_[idx].record_index = record_index;
    buckets_[idx].occupied = true;
    size_++;
}

std::vector<size_t> HashIndex::lookup(uint32_t key) const {
    std::vector<size_t> results;
    if (buckets_.empty()) return results;

    size_t idx = static_cast<size_t>(key) % buckets_.size();
    size_t start = idx;

    while (buckets_[idx].occupied) {
        if (buckets_[idx].key == key) {
            results.push_back(buckets_[idx].record_index);
        }
        idx = (idx + 1) % buckets_.size();
        if (idx == start) break;
    }

    return results;
}

} // namespace vde
