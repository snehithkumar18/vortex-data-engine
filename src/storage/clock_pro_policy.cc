#include "vde/storage/clock_pro_policy.h"

namespace vde {

ClockProPolicy::ClockProPolicy(size_t capacity)
    : capacity_(capacity == 0 ? 4 : capacity),
      hot_capacity_(capacity_ / 2) {}

void ClockProPolicy::access_page(uint32_t page_id) {
    auto it = page_map_.find(page_id);
    if (it != page_map_.end()) {
        size_t idx = it->second;
        hand_cold_[idx].referenced = true;
    } else {
        ClockProPageEntry entry{page_id, PageState::Cold, false};
        size_t idx = hand_cold_.size();
        hand_cold_.push_back(entry);
        page_map_[page_id] = idx;
        cold_count_++;
    }
}

uint32_t ClockProPolicy::evict_page() {
    for (size_t i = 0; i < hand_cold_.size(); ++i) {
        if (hand_cold_[i].state == PageState::Cold) {
            if (!hand_cold_[i].referenced) {
                uint32_t evicted_id = hand_cold_[i].page_id;
                page_map_.erase(evicted_id);
                hand_cold_.erase(hand_cold_.begin() + i);
                cold_count_--;
                return evicted_id;
            } else {
                hand_cold_[i].referenced = false;
                hand_cold_[i].state = PageState::Hot;
                hot_count_++;
                cold_count_--;
            }
        }
    }

    if (!hand_cold_.empty()) {
        uint32_t evicted_id = hand_cold_.front().page_id;
        page_map_.erase(evicted_id);
        hand_cold_.erase(hand_cold_.begin());
        return evicted_id;
    }
    return 0;
}

}
