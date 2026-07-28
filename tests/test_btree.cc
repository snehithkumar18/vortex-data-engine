#include "tests/test_framework.h"
#include "vde/query/btree_index.h"

TEST(btree_index_insert_search) {
    vde::BTreeIndex btree(4);
    btree.insert(10, 100);
    btree.insert(20, 200);
    btree.insert(15, 150);

    auto res = btree.search(15);
    ASSERT_EQ(res.size(), 1u);
    ASSERT_EQ(res[0], 150u);
}

RUN_ALL_TESTS()
