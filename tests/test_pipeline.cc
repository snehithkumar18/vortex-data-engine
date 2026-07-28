#include "tests/test_framework.h"
#include "vde/pipeline/pipeline.h"

TEST(pipeline_init) {
    vde::Pipeline pipeline;
    ASSERT_EQ(pipeline.records().record_count(), 0u);
}

RUN_ALL_TESTS()
