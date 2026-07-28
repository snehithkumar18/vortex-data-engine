#include "tests/test_framework.h"
#include "vde/common/skiplist.h"

TEST(skiplist_insert_search) {
    vde::SkipList sl;
    sl.insert(10, 100);
    sl.insert(20, 200);

    uint32_t val = 0;
    ASSERT_TRUE(sl.search(10, &val));
    ASSERT_EQ(val, 100u);

    ASSERT_TRUE(sl.search(20, &val));
    ASSERT_EQ(val, 200u);
}

RUN_ALL_TESTS()
