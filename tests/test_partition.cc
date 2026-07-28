#include "tests/test_framework.h"
#include "vde/storage/partition_manager.h"

TEST(partition_manager_range) {
    vde::PartitionManager pm("orders", vde::PartitionType::Range, 0);
    pm.add_partition({1, "p0", 0, 100});
    pm.add_partition({2, "p1", 101, 200});

    vde::Record rec;
    rec.id = 1;
    rec.fields.emplace_back(uint32_t(50));
    uint32_t pid = pm.resolve_partition(rec);
    ASSERT_EQ(pid, 1u);
}

RUN_ALL_TESTS()
