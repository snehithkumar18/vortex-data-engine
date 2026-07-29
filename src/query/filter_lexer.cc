#include "vde/query/filter_lexer.h"
#include <cctype>
#include <algorithm>

namespace vde {

FilterLexer::FilterLexer(std::string_view query)
    : source_(query), pos_(0), line_(1), col_(1) {}

char FilterLexer::peek() const {
    if (is_at_end()) return '\0';
    return source_[pos_];
}

char FilterLexer::advance() {
    if (is_at_end()) return '\0';
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        col_ = 1;
    } else {
        col_++;
    }
    return c;
}

bool FilterLexer::is_at_end() const {
    return pos_ >= source_.size();
}

void FilterLexer::skip_whitespace() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '-' && pos_ + 1 < source_.size() && source_[pos_ + 1] == '-') {

            while (!is_at_end() && peek() != '\n') advance();
        } else {
            break;
        }
    }
}

Token FilterLexer::make_token(TokenType type, std::string text) {
    Token t;
    t.type = type;
    t.text = std::move(text);
    t.line = line_;
    t.column = col_;
    return t;
}

Token FilterLexer::lex_identifier_or_keyword() {
    size_t start = pos_;
    while (!is_at_end() && (std::isalnum(peek()) || peek() == '_' || peek() == '.')) {
        advance();
    }
    std::string text(source_.substr(start, pos_ - start));
    std::string upper_text = text;
    std::transform(upper_text.begin(), upper_text.end(), upper_text.begin(), ::toupper);

    TokenType type = TokenType::Identifier;
    if (upper_text == "SELECT") type = TokenType::Select;
    else if (upper_text == "FROM") type = TokenType::From;
    else if (upper_text == "WHERE") type = TokenType::Where;
    else if (upper_text == "AND") type = TokenType::And;
    else if (upper_text == "OR") type = TokenType::Or;
    else if (upper_text == "NOT") type = TokenType::Not;
    else if (upper_text == "ORDER") type = TokenType::Order;
    else if (upper_text == "BY") type = TokenType::By;
    else if (upper_text == "GROUP") type = TokenType::Group;
    else if (upper_text == "LIMIT") type = TokenType::Limit;
    else if (upper_text == "ASC") type = TokenType::Asc;
    else if (upper_text == "DESC") type = TokenType::Desc;
    else if (upper_text == "AS") type = TokenType::As;

    return make_token(type, text);
}

Token FilterLexer::lex_number() {
    size_t start = pos_;
    bool is_float = false;

    while (!is_at_end() && (std::isdigit(peek()) || peek() == '.')) {
        if (peek() == '.') is_float = true;
        advance();
    }

    std::string text(source_.substr(start, pos_ - start));
    Token t = make_token(TokenType::NumberLiteral, text);
    if (is_float) {
        t.float_val = std::stod(text);
    } else {
        t.int_val = std::stoull(text);
    }
    return t;
}

Token FilterLexer::lex_string() {
    advance();
    size_t start = pos_;
    while (!is_at_end() && peek() != '\'') {
        advance();
    }
    std::string text(source_.substr(start, pos_ - start));
    if (!is_at_end()) advance();
    return make_token(TokenType::StringLiteral, text);
}

std::vector<Token> FilterLexer::tokenize() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        skip_whitespace();
        if (is_at_end()) break;

        char c = peek();
        if (std::isalpha(c) || c == '_') {
            tokens.push_back(lex_identifier_or_keyword());
        } else if (std::isdigit(c)) {
            tokens.push_back(lex_number());
        } else if (c == '\'') {
            tokens.push_back(lex_string());
        } else {
            advance();
            switch (c) {
                case '=': tokens.push_back(make_token(TokenType::Equal, "=")); break;
                case '!':
                    if (peek() == '=') { advance(); tokens.push_back(make_token(TokenType::NotEqual, "!=")); }
                    else tokens.push_back(make_token(TokenType::Invalid, "!"));
                    break;
                case '<':
                    if (peek() == '=') { advance(); tokens.push_back(make_token(TokenType::LessEqual, "<=")); }
                    else if (peek() == '>') { advance(); tokens.push_back(make_token(TokenType::NotEqual, "<>")); }
                    else tokens.push_back(make_token(TokenType::LessThan, "<"));
                    break;
                case '>':
                    if (peek() == '=') { advance(); tokens.push_back(make_token(TokenType::GreaterEqual, ">=")); }
                    else tokens.push_back(make_token(TokenType::GreaterThan, ">"));
                    break;
                case '+': tokens.push_back(make_token(TokenType::Plus, "+")); break;
                case '-': tokens.push_back(make_token(TokenType::Minus, "-")); break;
                case '*': tokens.push_back(make_token(TokenType::Star, "*")); break;
                case '/': tokens.push_back(make_token(TokenType::Slash, "/")); break;
                case ',': tokens.push_back(make_token(TokenType::Comma, ",")); break;
                case '(': tokens.push_back(make_token(TokenType::LParen, "(")); break;
                case ')': tokens.push_back(make_token(TokenType::RParen, ")")); break;
                default: tokens.push_back(make_token(TokenType::Invalid, std::string(1, c))); break;
            }
        }
    }
    tokens.push_back(make_token(TokenType::Eof, ""));
    return tokens;
}

}
