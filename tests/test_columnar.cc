#include "tests/test_framework.h"
#include "vde/record/columnar_storage.h"

TEST(columnar_batch_creation) {
    vde::RecordBatch batch;
    vde::Record rec1;
    rec1.fields.emplace_back(uint32_t(42));
    batch.add_record(std::move(rec1));

    vde::ColumnarBatch cbatch = vde::ColumnarBatch::from_record_batch(batch);
    ASSERT_EQ(cbatch.column_count(), 1u);
    ASSERT_EQ(cbatch.row_count(), 1u);

    const auto* col = cbatch.get_column("field0");
    ASSERT_NE(col, nullptr);
}

RUN_ALL_TESTS()
