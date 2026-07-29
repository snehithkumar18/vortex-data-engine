#pragma once

#include "vde/common/types.h"
#include <string>
#include <vector>

namespace vde {

enum class TokenType {
    Select,
    From,
    Where,
    And,
    Or,
    Not,
    Order,
    By,
    Group,
    Limit,
    Asc,
    Desc,
    As,
    Identifier,
    NumberLiteral,
    StringLiteral,
    Equal,
    NotEqual,
    LessThan,
    LessEqual,
    GreaterThan,
    GreaterEqual,
    Plus,
    Minus,
    Star,
    Slash,
    Comma,
    LParen,
    RParen,
    Eof,
    Invalid
};

struct Token {
    TokenType type;
    std::string text;
    size_t line;
    size_t column;
    uint64_t int_val = 0;
    double float_val = 0.0;
};

class FilterLexer {
public:
    explicit FilterLexer(std::string_view query);

    std::vector<Token> tokenize();

private:
    char peek() const;
    char advance();
    bool is_at_end() const;
    void skip_whitespace();

    Token make_token(TokenType type, std::string text);
    Token lex_identifier_or_keyword();
    Token lex_number();
    Token lex_string();

    std::string_view source_;
    size_t pos_ = 0;
    size_t line_ = 1;
    size_t col_ = 1;
};

}
