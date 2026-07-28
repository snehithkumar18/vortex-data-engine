#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdint>

namespace vde {

class BloomFilter {
public:
    explicit BloomFilter(size_t bit_size = 8192, size_t hash_count = 3);
    ~BloomFilter() = default;

    void add(Span<const byte_t> data);
    bool possibly_contains(Span<const byte_t> data) const;
    void clear();

    size_t bit_size() const { return bit_size_; }

private:
    size_t bit_size_;
    size_t hash_count_;
    std::vector<uint64_t> bitmap_;
};

} // namespace vde
