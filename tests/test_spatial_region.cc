#include "tests/test_framework.h"
#include "vde/query/spatial_region_index.h"

TEST(spatial_region_index_query) {
    vde::SpatialRegionIndex spatial_region;
    vde::BoundingBox b1{0.0, 0.0, 10.0, 10.0};
    spatial_region.insert(b1, 100);

    vde::BoundingBox q{5.0, 5.0, 15.0, 15.0};
    auto res = spatial_region.query_range(q);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], 100u);
}

RUN_ALL_TESTS()
