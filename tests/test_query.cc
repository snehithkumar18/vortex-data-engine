#include "tests/test_framework.h"
#include "vde/query/evaluator.h"

TEST(query_evaluator_literal) {
    auto lit = vde::ExprNode::make_literal(vde::FieldValue(uint32_t(1)));
    vde::Record rec;
    vde::QueryEvaluator eval;
    bool res = eval.evaluate(lit, rec);
    ASSERT_TRUE(res);
    vde::free_expression(lit);
}

RUN_ALL_TESTS()
