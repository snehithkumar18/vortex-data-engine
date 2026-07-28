#include "tests/test_framework.h"
#include "vde/storage/lsm_tree.h"

TEST(lsm_tree_put_get) {
    vde::LsmTreeEngine lsm("./lsm_db", 10);
    lsm.put(1, 100);
    lsm.put(2, 200);

    uint32_t val = 0;
    ASSERT_TRUE(lsm.get(1, &val));
    ASSERT_EQ(val, 100u);
}

RUN_ALL_TESTS()
