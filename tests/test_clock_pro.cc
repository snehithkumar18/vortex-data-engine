#include "tests/test_framework.h"
#include "vde/storage/clock_pro_policy.h"

TEST(clock_pro_access_evict) {
    vde::ClockProPolicy policy(4);
    policy.access_page(1);
    policy.access_page(2);
    policy.access_page(3);

    ASSERT_EQ(policy.cold_count(), 3u);

    uint32_t evicted = policy.evict_page();
    ASSERT_EQ(evicted, 1u);
}

RUN_ALL_TESTS()
