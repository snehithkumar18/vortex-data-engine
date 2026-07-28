#include "vde/common/hyperloglog.h"
#include "vde/common/math_utils.h"
#include <algorithm>

namespace vde {

HyperLogLog::HyperLogLog(uint8_t precision)
    : p_(precision < 4 ? 4 : (precision > 16 ? 16 : precision)) {
    m_ = 1ULL << p_;
    registers_.resize(m_, 0);

    if (m_ == 16) alpha_m_ = 0.673;
    else if (m_ == 32) alpha_m_ = 0.697;
    else if (m_ == 64) alpha_m_ = 0.709;
    else alpha_m_ = 0.7213 / (1.0 + 1.079 / static_cast<double>(m_));
}

uint8_t HyperLogLog::get_leading_zeros(uint64_t hash, uint8_t max_bits) const {
    uint8_t zeros = 1;
    while ((hash & 1) == 0 && zeros <= max_bits) {
        zeros++;
        hash >>= 1;
    }
    return zeros;
}

void HyperLogLog::clear() {
    std::fill(registers_.begin(), registers_.end(), 0);
}

void HyperLogLog::add(Span<const byte_t> data) {
    uint64_t hash = fnv1a_64(data);
    size_t idx = hash >> (64 - p_);
    uint64_t w = hash << p_;
    uint8_t lz = get_leading_zeros(w, 64 - p_);

    if (lz > registers_[idx]) {
        registers_[idx] = lz;
    }
}

uint64_t HyperLogLog::estimate_cardinality() const {
    double sum = 0.0;
    size_t zeros = 0;

    for (uint8_t val : registers_) {
        sum += 1.0 / (1ULL << val);
        if (val == 0) zeros++;
    }

    double E = alpha_m_ * static_cast<double>(m_) * static_cast<double>(m_) / sum;

    if (E <= 2.5 * static_cast<double>(m_)) {
        if (zeros > 0) {
            E = static_cast<double>(m_) * std::log(static_cast<double>(m_) / static_cast<double>(zeros));
        }
    }
    return static_cast<uint64_t>(E);
}

void HyperLogLog::merge(const HyperLogLog& other) {
    if (p_ != other.p_) return;
    for (size_t i = 0; i < m_; ++i) {
        registers_[i] = std::max(registers_[i], other.registers_[i]);
    }
}

} // namespace vde
