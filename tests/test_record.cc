#include "tests/test_framework.h"
#include "vde/record/field_value.h"
#include "vde/record/record_batch.h"

TEST(field_value_primitives) {
    vde::FieldValue v1(uint32_t(100));
    ASSERT_EQ(v1.as_u32(), 100u);

    vde::FieldValue v2("hello", 5);
    size_t len = 0;
    const char* str = v2.as_string(&len);
    ASSERT_EQ(len, 5u);
    ASSERT_NE(str, nullptr);
}

TEST(record_batch_add) {
    vde::RecordBatch batch;
    vde::Record r;
    r.id = 1;
    batch.add_record(std::move(r));
    ASSERT_EQ(batch.record_count(), 1u);
}

RUN_ALL_TESTS()
