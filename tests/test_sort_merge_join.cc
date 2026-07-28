#include "tests/test_framework.h"
#include "vde/query/sort_merge_join.h"

TEST(sort_merge_join_init) {
    vde::SortMergeJoinOperator op(nullptr, nullptr, 0, 0);
    ASSERT_EQ(static_cast<int>(op.open()), static_cast<int>(vde::Status::InvalidArgument));
}

RUN_ALL_TESTS()
