#include "vde/common/skiplist.h"

namespace vde {

SkipList::SkipList(int max_level)
    : max_level_(max_level), level_(0), size_(0) {
    head_ = new SkipNode(0, 0, max_level_);
}

SkipList::~SkipList() {
    SkipNode* curr = head_;
    while (curr) {
        SkipNode* next = curr->forward[0];
        delete curr;
        curr = next;
    }
}

int SkipList::random_level() const {
    int lvl = 0;
    while ((std::rand() & 1) == 1 && lvl < max_level_) {
        lvl++;
    }
    return lvl;
}

void SkipList::insert(uint32_t key, uint32_t value) {
    std::vector<SkipNode*> update(max_level_ + 1, nullptr);
    SkipNode* curr = head_;

    for (int i = level_; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }

    curr = curr->forward[0];
    if (curr && curr->key == key) {
        curr->value = value;
        return;
    }

    int new_level = random_level();
    if (new_level > level_) {
        for (int i = level_ + 1; i <= new_level; i++) {
            update[i] = head_;
        }
        level_ = new_level;
    }

    SkipNode* new_node = new SkipNode(key, value, new_level);
    for (int i = 0; i <= new_level; i++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }
    size_++;
}

bool SkipList::search(uint32_t key, uint32_t* out_value) const {
    SkipNode* curr = head_;
    for (int i = level_; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
    }
    curr = curr->forward[0];
    if (curr && curr->key == key) {
        if (out_value) *out_value = curr->value;
        return true;
    }
    return false;
}

bool SkipList::remove(uint32_t key) {
    std::vector<SkipNode*> update(max_level_ + 1, nullptr);
    SkipNode* curr = head_;

    for (int i = level_; i >= 0; i--) {
        while (curr->forward[i] && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }

    curr = curr->forward[0];
    if (!curr || curr->key != key) return false;

    for (int i = 0; i <= level_; i++) {
        if (update[i]->forward[i] != curr) break;
        update[i]->forward[i] = curr->forward[i];
    }

    delete curr;
    while (level_ > 0 && head_->forward[level_] == nullptr) {
        level_--;
    }

    size_--;
    return true;
}

} // namespace vde
