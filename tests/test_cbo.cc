#include "tests/test_framework.h"
#include "vde/query/cost_model.h"

TEST(equi_height_histogram) {
    vde::EquiHeightHistogram hist;
    std::vector<double> samples = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
    hist.build(samples, 2);

    ASSERT_EQ(hist.bucket_count(), 2u);
    ASSERT_EQ(hist.total_samples(), 10u);
}

RUN_ALL_TESTS()
