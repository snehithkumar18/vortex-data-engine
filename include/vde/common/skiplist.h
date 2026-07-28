#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdlib>

namespace vde {

struct SkipNode {
    uint32_t key;
    uint32_t value;
    std::vector<SkipNode*> forward;

    SkipNode(uint32_t k, uint32_t v, int level)
        : key(k), value(v), forward(level + 1, nullptr) {}
};

class SkipList {
public:
    explicit SkipList(int max_level = 16);
    ~SkipList();

    void insert(uint32_t key, uint32_t value);
    bool search(uint32_t key, uint32_t* out_value) const;
    bool remove(uint32_t key);

    size_t size() const { return size_; }

private:
    int random_level() const;

    int max_level_;
    int level_ = 0;
    SkipNode* head_;
    size_t size_ = 0;
};

} // namespace vde
