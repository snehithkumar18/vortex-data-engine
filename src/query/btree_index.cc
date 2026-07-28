#include "vde/query/btree_index.h"
#include <algorithm>

namespace vde {

BTreeIndex::BTreeIndex(size_t max_keys)
    : root_(std::make_unique<BTreeNode>()), max_keys_(max_keys < 3 ? 3 : max_keys) {}

void BTreeIndex::insert(uint32_t key, size_t record_index) {
    total_keys_++;
    if (root_->keys.size() == max_keys_) {
        auto new_root = std::make_unique<BTreeNode>();
        new_root->is_leaf = false;
        new_root->children.push_back(std::move(root_));
        split_child(new_root.get(), 0, new_root->children[0].get());
        root_ = std::move(new_root);
    }
    insert_non_full(root_.get(), key, record_index);
}

void BTreeIndex::split_child(BTreeNode* parent, size_t index, BTreeNode* child) {
    auto sibling = std::make_unique<BTreeNode>();
    sibling->is_leaf = child->is_leaf;

    size_t mid = child->keys.size() / 2;
    uint32_t median_key = child->keys[mid];

    for (size_t i = mid + 1; i < child->keys.size(); ++i) {
        sibling->keys.push_back(child->keys[i]);
        if (child->is_leaf) sibling->record_indices.push_back(child->record_indices[i]);
    }

    if (!child->is_leaf) {
        for (size_t i = mid + 1; i < child->children.size(); ++i) {
            sibling->children.push_back(std::move(child->children[i]));
        }
        child->children.resize(mid + 1);
    }

    child->keys.resize(mid);
    if (child->is_leaf) child->record_indices.resize(mid);

    parent->children.insert(parent->children.begin() + index + 1, std::move(sibling));
    parent->keys.insert(parent->keys.begin() + index, median_key);
}

void BTreeIndex::insert_non_full(BTreeNode* node, uint32_t key, size_t record_index) {
    if (node->is_leaf) {
        auto it = std::upper_bound(node->keys.begin(), node->keys.end(), key);
        size_t idx = std::distance(node->keys.begin(), it);
        node->keys.insert(it, key);
        node->record_indices.insert(node->record_indices.begin() + idx, record_index);
    } else {
        auto it = std::upper_bound(node->keys.begin(), node->keys.end(), key);
        size_t idx = std::distance(node->keys.begin(), it);
        if (node->children[idx]->keys.size() == max_keys_) {
            split_child(node, idx, node->children[idx].get());
            if (key > node->keys[idx]) idx++;
        }
        insert_non_full(node->children[idx].get(), key, record_index);
    }
}

std::vector<size_t> BTreeIndex::search(uint32_t key) const {
    std::vector<size_t> results;
    search_recursive(root_.get(), key, results);
    return results;
}

void BTreeIndex::search_recursive(const BTreeNode* node, uint32_t key, std::vector<size_t>& results) const {
    if (!node) return;
    for (size_t i = 0; i < node->keys.size(); ++i) {
        if (!node->is_leaf) {
            if (key <= node->keys[i]) {
                search_recursive(node->children[i].get(), key, results);
            }
        }
        if (node->keys[i] == key && node->is_leaf) {
            results.push_back(node->record_indices[i]);
        }
    }
    if (!node->is_leaf && !node->children.empty()) {
        search_recursive(node->children.back().get(), key, results);
    }
}

}
