#include "tests/test_framework.h"
#include "vde/query/vectorized_executor.h"

TEST(vectorized_selection_vector) {
    vde::SelectionVector sel;
    sel.indices.push_back(10);
    sel.indices.push_back(20);
    ASSERT_EQ(sel.size(), 2u);
}

RUN_ALL_TESTS()
