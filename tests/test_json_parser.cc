#include "tests/test_framework.h"
#include "vde/record/json_parser.h"

TEST(json_parser_basic) {
    vde::JsonRecordParser parser;
    auto res = parser.parse_record("{}");
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value.id, 1u);
}

RUN_ALL_TESTS()
