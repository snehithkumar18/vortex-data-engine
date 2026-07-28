#include "vde/common/math_utils.h"

namespace vde {

uint32_t murmurhash3_32(Span<const byte_t> data, uint32_t seed) {
    uint32_t h1 = seed;
    size_t len = data.size();
    size_t nblocks = len / 4;

    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data.data());
    for (size_t i = 0; i < nblocks; ++i) {
        uint32_t k1 = blocks[i];
        k1 *= 0xcc9e2d51;
        k1 = (k1 << 15) | (k1 >> 17);
        k1 *= 0x1b873593;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);
        h1 = h1 * 5 + 0xe6546b64;
    }

    const uint8_t* tail = data.data() + nblocks * 4;
    uint32_t k1 = 0;
    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16; [[fallthrough]];
        case 2: k1 ^= tail[1] << 8;  [[fallthrough]];
        case 1: k1 ^= tail[0];
                k1 *= 0xcc9e2d51;
                k1 = (k1 << 15) | (k1 >> 17);
                k1 *= 0x1b873593;
                h1 ^= k1;
    }

    h1 ^= static_cast<uint32_t>(len);
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

uint64_t fnv1a_64(Span<const byte_t> data) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < data.size(); ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t xxhash_64_lite(Span<const byte_t> data, uint64_t seed) {
    uint64_t h = seed + 11400714785074694791ULL + data.size();
    for (size_t i = 0; i < data.size(); ++i) {
        h ^= static_cast<uint64_t>(data[i]);
        h *= 14029467366897019727ULL;
        h = (h << 31) | (h >> 33);
    }
    return h;
}

uint32_t align_up(uint32_t val, uint32_t alignment) {
    if (alignment == 0) return val;
    return (val + alignment - 1) & ~(alignment - 1);
}

uint64_t align_up(uint64_t val, uint64_t alignment) {
    if (alignment == 0) return val;
    return (val + alignment - 1) & ~(alignment - 1);
}

bool is_power_of_two(uint64_t val) {
    return val > 0 && (val & (val - 1)) == 0;
}

uint32_t count_leading_zeros_32(uint32_t val) {
    if (val == 0) return 32;
    uint32_t count = 0;
    if ((val & 0xFFFF0000) == 0) { count += 16; val <<= 16; }
    if ((val & 0xFF000000) == 0) { count += 8; val <<= 8; }
    if ((val & 0xF0000000) == 0) { count += 4; val <<= 4; }
    if ((val & 0xC0000000) == 0) { count += 2; val <<= 2; }
    if ((val & 0x80000000) == 0) { count += 1; }
    return count;
}

uint32_t count_trailing_zeros_32(uint32_t val) {
    if (val == 0) return 32;
    uint32_t count = 0;
    if ((val & 0x0000FFFF) == 0) { count += 16; val >>= 16; }
    if ((val & 0x000000FF) == 0) { count += 8; val >>= 8; }
    if ((val & 0x0000000F) == 0) { count += 4; val >>= 4; }
    if ((val & 0x00000003) == 0) { count += 2; val >>= 2; }
    if ((val & 0x00000001) == 0) { count += 1; }
    return count;
}

uint32_t popcount_32(uint32_t val) {
    val = val - ((val >> 1) & 0x55555555);
    val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
    return (((val + (val >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

int64_t zigzag_encode_64(int64_t val) {
    return (val << 1) ^ (val >> 63);
}

int64_t zigzag_decode_64(uint64_t val) {
    return static_cast<int64_t>((val >> 1) ^ (-(val & 1)));
}

SimpleRandom::SimpleRandom(uint64_t seed) : state_(seed) {}

uint64_t SimpleRandom::next_u64() {
    uint64_t z = (state_ += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

uint32_t SimpleRandom::next_u32() {
    return static_cast<uint32_t>(next_u64() >> 32);
}

double SimpleRandom::next_double() {
    return static_cast<double>(next_u64() >> 11) * (1.0 / 9007199254740992.0);
}

}
