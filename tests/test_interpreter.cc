#include "tests/test_framework.h"
#include "vde/query/expression_interpreter.h"

TEST(expression_interpreter_literal) {
    vde::ExpressionInterpreter interp;
    auto lit = vde::ExprNode::make_literal(vde::FieldValue(uint32_t(99)));
    vde::Record rec;
    auto res = interp.evaluate_node(lit, rec);

    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value.as_u32(), 99u);
    vde::free_expression(lit);
}

RUN_ALL_TESTS()
