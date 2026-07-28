#include "tests/test_framework.h"
#include "vde/query/hash_aggregate.h"

TEST(hash_aggregate_init) {
    vde::HashAggregateOperator op(nullptr, 0, 1, vde::AggregateType::Sum);
    ASSERT_EQ(static_cast<int>(op.open()), static_cast<int>(vde::Status::InvalidArgument));
}

RUN_ALL_TESTS()
