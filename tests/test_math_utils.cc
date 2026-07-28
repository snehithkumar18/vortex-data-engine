#include "tests/test_framework.h"
#include "vde/common/math_utils.h"

TEST(math_utils_murmurhash) {
    vde::byte_t data[] = "test_data";
    vde::Span<const vde::byte_t> span(data, 9);
    uint32_t h = vde::murmurhash3_32(span);
    ASSERT_NE(h, 0u);
}

TEST(math_utils_bitops) {
    ASSERT_EQ(vde::popcount_32(0x0F0F), 8u);
    ASSERT_TRUE(vde::is_power_of_two(1024));
    ASSERT_FALSE(vde::is_power_of_two(1023));
}

RUN_ALL_TESTS()
