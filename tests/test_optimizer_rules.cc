#include "tests/test_framework.h"
#include "vde/query/optimizer_rules.h"

TEST(optimizer_rules_apply) {
    vde::RuleBasedOptimizer opt;
    vde::FilterQueryNode stmt;
    vde::Status st = opt.optimize(&stmt);
    ASSERT_EQ(static_cast<int>(st), static_cast<int>(vde::Status::Ok));
}

RUN_ALL_TESTS()
