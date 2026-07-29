#pragma once

#include "vde/query/filter_lexer.h"
#include "vde/query/expression.h"
#include <memory>
#include <vector>
#include <string>

namespace vde {

struct SelectStatement {
    std::vector<std::string> projection_fields;
    std::string from_table;
    ExprNode* where_clause = nullptr;
    std::string order_by_field;
    bool order_desc = false;
    size_t limit = 0;

    ~SelectStatement() {
        delete where_clause;
    }
};

class FilterParser {
public:
    explicit FilterParser(std::vector<Token> tokens);

    Result<std::unique_ptr<SelectStatement>> parse_select();

private:
    const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
    bool check(TokenType type) const;

    ExprNode* parse_expression();
    ExprNode* parse_or();
    ExprNode* parse_and();
    ExprNode* parse_equality();
    ExprNode* parse_comparison();
    ExprNode* parse_primary();

    std::vector<Token> tokens_;
    size_t current_ = 0;
};

}
