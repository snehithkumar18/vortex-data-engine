#pragma once

#include "vde/common/types.h"
#include "vde/record/field_value.h"
#include <vector>
#include <memory>

namespace vde {

struct BTreeNode {
    bool is_leaf = true;
    std::vector<uint32_t> keys;
    std::vector<size_t> record_indices;
    std::vector<std::unique_ptr<BTreeNode>> children;
};

class BTreeIndex {
public:
    explicit BTreeIndex(size_t max_keys = 8);
    ~BTreeIndex() = default;

    void insert(uint32_t key, size_t record_index);
    std::vector<size_t> search(uint32_t key) const;

    size_t size() const { return total_keys_; }

private:
    void insert_non_full(BTreeNode* node, uint32_t key, size_t record_index);
    void split_child(BTreeNode* parent, size_t index, BTreeNode* child);
    void search_recursive(const BTreeNode* node, uint32_t key, std::vector<size_t>& results) const;

    std::unique_ptr<BTreeNode> root_;
    size_t max_keys_;
    size_t total_keys_ = 0;
};

} // namespace vde
