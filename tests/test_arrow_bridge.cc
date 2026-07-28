#include "tests/test_framework.h"
#include "vde/record/arrow_bridge.h"

TEST(arrow_bridge_export) {
    vde::Uint32ColumnVector col("col1");
    col.append(10);
    col.append(20);

    vde::ArrowArrayBridge arr;
    vde::ArrowSchemaBridge sch;
    vde::Status st = vde::ArrowDataBridge::export_column(col, &arr, &sch);

    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
    ASSERT_EQ(arr.length, 2);
}

RUN_ALL_TESTS()
