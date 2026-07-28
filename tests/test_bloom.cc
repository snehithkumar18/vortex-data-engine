#include "tests/test_framework.h"
#include "vde/common/bloom_filter.h"

TEST(bloom_filter_basic) {
    vde::BloomFilter bf(1024, 3);
    vde::byte_t k1[] = "key_1";
    vde::byte_t k2[] = "key_2";

    bf.add(vde::Span<const vde::byte_t>(k1, 5));
    ASSERT_TRUE(bf.possibly_contains(vde::Span<const vde::byte_t>(k1, 5)));
    ASSERT_FALSE(bf.possibly_contains(vde::Span<const vde::byte_t>(k2, 5)));
}

RUN_ALL_TESTS()
