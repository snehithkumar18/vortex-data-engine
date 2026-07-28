#include "tests/test_framework.h"
#include "vde/record/parquet_converter.h"

TEST(parquet_converter_basic) {
    vde::ParquetConverter conv;
    vde::ColumnarBatch batch;
    vde::OwnedBuffer buf = conv.convert_to_parquet(batch);
    ASSERT_EQ(buf.size(), 16u);
}

RUN_ALL_TESTS()
