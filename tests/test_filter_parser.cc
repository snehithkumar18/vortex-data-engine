#include "tests/test_framework.h"
#include "vde/query/filter_lexer.h"
#include "vde/query/filter_parser.h"

TEST(filter_lexer_basic) {
    vde::FilterLexer lexer("SELECT field1 FROM table WHERE field2 = 10");
    auto tokens = lexer.tokenize();
    ASSERT_EQ(tokens.size(), 9u);
    ASSERT_EQ(static_cast<int>(tokens[0].type), static_cast<int>(vde::TokenType::Select));
    ASSERT_EQ(static_cast<int>(tokens[2].type), static_cast<int>(vde::TokenType::From));
}

TEST(filter_parser_basic_select) {
    vde::FilterLexer lexer("SELECT field1 FROM table WHERE field2 = 10 LIMIT 5");
    auto tokens = lexer.tokenize();
    vde::FilterParser parser(std::move(tokens));
    auto res = parser.parse_select();
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value->limit, 5u);
}

RUN_ALL_TESTS()
