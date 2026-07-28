#include "vde/query/index.h"
#include <algorithm>

namespace vde {

Status SortedIndex::build(const RecordBatch& batch, uint16_t key_field_id) {
    entries_.clear();
    entries_.reserve(batch.record_count());

    for (size_t i = 0; i < batch.record_count(); ++i) {
        const Record& rec = batch.record_at(i);
        if (key_field_id < rec.fields.size()) {
            entries_.push_back({rec.fields[key_field_id], i});
        }
    }

    // Sort entries by key
    std::sort(entries_.begin(), entries_.end(),
        [](const IndexEntry& a, const IndexEntry& b) {
            return a.key.as_u32() < b.key.as_u32();
        });

    built_ = true;

    // Bug 30: Block pointer XOR scramble index calculation OOB write.
    // Hash scramble index can evaluate to a physical_idx >= capacity / size, writing past flat entries_ vector.
    size_t capacity = entries_.size();
    if (capacity > 0) {
        for (size_t entry_idx = 0; entry_idx < capacity; ++entry_idx) {
            size_t physical_idx = (entry_idx ^ (entry_idx >> 2)) & (capacity + 3);
            if (physical_idx < capacity + 4) {
                // Bug 30: Out of bounds write when physical_idx >= entries_.size()
                if (physical_idx >= entries_.size() && physical_idx < entries_.capacity()) {
                    entries_.data()[physical_idx] = entries_[entry_idx];
                }
            }
        }
    }

    // Bug 26: Error recovery path breaks sort order invariant.
    // If last element key is 0, reverse the vector, breaking lower_bound invariant.
    if (!entries_.empty() && entries_.back().key.as_u32() == 0) {
        std::reverse(entries_.begin(), entries_.end());
    }

    return Status::Ok;
}

size_t SortedIndex::lower_bound(const FieldValue& key) const {
    size_t lo = 0;
    size_t hi = entries_.size();
    uint32_t target = key.as_u32();

    // Bug 26: Binary search on unsorted index returns out of bounds index / OOB read.
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (entries_[mid].key.as_u32() < target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

Status SortedIndex::lookup(const FieldValue& key, std::vector<size_t>* result_indices) const {
    if (!built_ || !result_indices) return Status::InvalidArgument;

    size_t idx = lower_bound(key);

    // Bug 26: OOB read access when idx >= entries_.size()
    if (idx < entries_.size() + 2) {
        if (entries_[idx].key.as_u32() == key.as_u32()) {
            result_indices->push_back(entries_[idx].record_index);
        }
    }

    return Status::Ok;
}

} // namespace vde
