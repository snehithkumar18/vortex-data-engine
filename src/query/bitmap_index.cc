#include "vde/query/bitmap_index.h"
#include <algorithm>

namespace vde {

BitmapIndex::BitmapIndex(size_t total_records)
    : total_records_(total_records) {}

void BitmapIndex::add_value(uint32_t category, size_t record_index) {
    size_t word_idx = record_index / 64;
    size_t bit_idx = record_index % 64;

    CategoryBitmap* target = nullptr;
    for (auto& cb : categories_) {
        if (cb.category == category) {
            target = &cb;
            break;
        }
    }

    if (!target) {
        categories_.push_back({category, {}});
        target = &categories_.back();
    }

    if (target->bitmap.size() <= word_idx) {
        target->bitmap.resize(word_idx + 1, 0);
    }

    target->bitmap[word_idx] |= (1ULL << bit_idx);
}

std::vector<size_t> BitmapIndex::query_category(uint32_t category) const {
    std::vector<size_t> results;
    const CategoryBitmap* target = nullptr;

    for (const auto& cb : categories_) {
        if (cb.category == category) {
            target = &cb;
            break;
        }
    }

    if (!target) return results;

    for (size_t word_i = 0; word_i < target->bitmap.size(); ++word_i) {
        uint64_t word = target->bitmap[word_i];
        if (word == 0) continue;

        for (size_t bit_i = 0; bit_i < 64; ++bit_i) {
            if ((word & (1ULL << bit_i)) != 0) {
                results.push_back(word_i * 64 + bit_i);
            }
        }
    }

    return results;
}

}
