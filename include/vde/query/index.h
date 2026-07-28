#pragma once

#include "vde/record/record_batch.h"
#include <vector>

namespace vde {

class SortedIndex {
public:
    SortedIndex() = default;

    Status build(const RecordBatch& batch, uint16_t key_field_id);
    Status lookup(const FieldValue& key, std::vector<size_t>* result_indices) const;

    size_t entry_count() const { return entries_.size(); }
    bool is_built() const { return built_; }

private:
    struct IndexEntry {
        FieldValue key;
        size_t record_index;
    };

    size_t lower_bound(const FieldValue& key) const;

    std::vector<IndexEntry> entries_;
    bool built_ = false;
};

}
