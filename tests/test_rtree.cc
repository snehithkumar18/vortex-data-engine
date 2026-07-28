#include "tests/test_framework.h"
#include "vde/query/rtree_index.h"

TEST(rtree_index_query) {
    vde::RTreeIndex rtree;
    vde::BoundingBox b1{0.0, 0.0, 10.0, 10.0};
    rtree.insert(b1, 100);

    vde::BoundingBox q{5.0, 5.0, 15.0, 15.0};
    auto res = rtree.query_range(q);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], 100u);
}

RUN_ALL_TESTS()
