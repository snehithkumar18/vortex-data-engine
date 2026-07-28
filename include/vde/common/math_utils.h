#pragma once

#include "vde/common/types.h"
#include <cstdint>
#include <cstddef>
#include <string>

namespace vde {

uint32_t murmurhash3_32(Span<const byte_t> data, uint32_t seed = 42);
uint64_t fnv1a_64(Span<const byte_t> data);
uint64_t xxhash_64_lite(Span<const byte_t> data, uint64_t seed = 0);

uint32_t align_up(uint32_t val, uint32_t alignment);
uint64_t align_up(uint64_t val, uint64_t alignment);
bool is_power_of_two(uint64_t val);

uint32_t count_leading_zeros_32(uint32_t val);
uint32_t count_trailing_zeros_32(uint32_t val);
uint32_t popcount_32(uint32_t val);

int64_t zigzag_encode_64(int64_t val);
int64_t zigzag_decode_64(uint64_t val);

class SimpleRandom {
public:
    explicit SimpleRandom(uint64_t seed = 8817264546732525261ULL);
    uint64_t next_u64();
    uint32_t next_u32();
    double next_double();

private:
    uint64_t state_;
};

}
