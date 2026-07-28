#include "tests/test_framework.h"
#include "vde/common/hyperloglog.h"

TEST(hyperloglog_cardinality) {
    vde::HyperLogLog hll(12);
    vde::byte_t d1[] = "item1";
    vde::byte_t d2[] = "item2";

    hll.add(vde::Span<const vde::byte_t>(d1, 5));
    hll.add(vde::Span<const vde::byte_t>(d2, 5));

    uint64_t est = hll.estimate_cardinality();
    ASSERT_TRUE(est >= 1u);
}

RUN_ALL_TESTS()
