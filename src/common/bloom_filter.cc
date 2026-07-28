#include "vde/common/bloom_filter.h"
#include "vde/common/math_utils.h"

namespace vde {

BloomFilter::BloomFilter(size_t bit_size, size_t hash_count)
    : bit_size_(bit_size == 0 ? 64 : bit_size), hash_count_(hash_count) {
    bitmap_.resize((bit_size_ + 63) / 64, 0);
}

void BloomFilter::clear() {
    std::fill(bitmap_.begin(), bitmap_.end(), 0);
}

void BloomFilter::add(Span<const byte_t> data) {
    uint64_t h = fnv1a_64(data);
    uint32_t h1 = static_cast<uint32_t>(h);
    uint32_t h2 = static_cast<uint32_t>(h >> 32);

    for (size_t i = 0; i < hash_count_; ++i) {
        uint64_t combined = (h1 + i * h2) % bit_size_;
        size_t word_idx = combined / 64;
        size_t bit_idx = combined % 64;
        bitmap_[word_idx] |= (1ULL << bit_idx);
    }
}

bool BloomFilter::possibly_contains(Span<const byte_t> data) const {
    uint64_t h = fnv1a_64(data);
    uint32_t h1 = static_cast<uint32_t>(h);
    uint32_t h2 = static_cast<uint32_t>(h >> 32);

    for (size_t i = 0; i < hash_count_; ++i) {
        uint64_t combined = (h1 + i * h2) % bit_size_;
        size_t word_idx = combined / 64;
        size_t bit_idx = combined % 64;
        if ((bitmap_[word_idx] & (1ULL << bit_idx)) == 0) {
            return false;
        }
    }
    return true;
}

} // namespace vde
