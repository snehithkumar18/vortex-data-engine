#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cmath>

namespace vde {

class HyperLogLog {
public:
    explicit HyperLogLog(uint8_t precision = 12);
    ~HyperLogLog() = default;

    void add(Span<const byte_t> data);
    uint64_t estimate_cardinality() const;
    void merge(const HyperLogLog& other);
    void clear();

    uint8_t precision() const { return p_; }
    size_t register_count() const { return m_; }

private:
    uint8_t get_leading_zeros(uint64_t hash, uint8_t max_bits) const;

    uint8_t p_;
    size_t m_;
    double alpha_m_;
    std::vector<uint8_t> registers_;
};

} // namespace vde
