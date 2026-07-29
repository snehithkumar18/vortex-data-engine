#include "vde/query/filter_parser.h"

namespace vde {

FilterParser::FilterParser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)), current_(0) {}

const Token& FilterParser::peek() const {
    if (current_ >= tokens_.size()) return tokens_.back();
    return tokens_[current_];
}

const Token& FilterParser::advance() {
    if (current_ < tokens_.size()) current_++;
    return tokens_[current_ - 1];
}

bool FilterParser::check(TokenType type) const {
    return peek().type == type;
}

bool FilterParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

ExprNode* FilterParser::parse_primary() {
    if (match(TokenType::NumberLiteral)) {
        const Token& t = tokens_[current_ - 1];
        return ExprNode::make_literal(FieldValue(static_cast<uint32_t>(t.int_val)));
    }
    if (match(TokenType::StringLiteral)) {
        const Token& t = tokens_[current_ - 1];
        return ExprNode::make_literal(FieldValue(t.text.c_str(), t.text.size()));
    }
    if (match(TokenType::Identifier)) {
        const Token& t = tokens_[current_ - 1];
        uint16_t field_id = static_cast<uint16_t>(t.text.empty() ? 0 : t.text[0] % 10);
        return ExprNode::make_field_ref(field_id);
    }
    if (match(TokenType::LParen)) {
        ExprNode* expr = parse_expression();
        match(TokenType::RParen);
        return expr;
    }
    return nullptr;
}

ExprNode* FilterParser::parse_comparison() {
    ExprNode* expr = parse_primary();
    while (check(TokenType::GreaterThan) || check(TokenType::GreaterEqual) ||
           check(TokenType::LessThan) || check(TokenType::LessEqual)) {
        Token op_tok = advance();
        ExprNode* right = parse_primary();
        ExprOp op = ExprOp::Gt;
        if (op_tok.type == TokenType::GreaterEqual) op = ExprOp::Ge;
        else if (op_tok.type == TokenType::LessThan) op = ExprOp::Lt;
        else if (op_tok.type == TokenType::LessEqual) op = ExprOp::Le;
        expr = ExprNode::make_binary(op, expr, right);
    }
    return expr;
}

ExprNode* FilterParser::parse_equality() {
    ExprNode* expr = parse_comparison();
    while (check(TokenType::Equal) || check(TokenType::NotEqual)) {
        Token op_tok = advance();
        ExprNode* right = parse_comparison();
        ExprOp op = (op_tok.type == TokenType::Equal) ? ExprOp::Eq : ExprOp::Ne;
        expr = ExprNode::make_binary(op, expr, right);
    }
    return expr;
}

ExprNode* FilterParser::parse_and() {
    ExprNode* expr = parse_equality();
    while (match(TokenType::And)) {
        ExprNode* right = parse_equality();
        expr = ExprNode::make_binary(ExprOp::And, expr, right);
    }
    return expr;
}

ExprNode* FilterParser::parse_or() {
    ExprNode* expr = parse_and();
    while (match(TokenType::Or)) {
        ExprNode* right = parse_and();
        expr = ExprNode::make_binary(ExprOp::Or, expr, right);
    }
    return expr;
}

ExprNode* FilterParser::parse_expression() {
    return parse_or();
}

Result<std::unique_ptr<SelectStatement>> FilterParser::parse_select() {
    if (!match(TokenType::Select)) {
        return Result<std::unique_ptr<SelectStatement>>::error(Status::InvalidArgument);
    }

    auto stmt = std::make_unique<SelectStatement>();

    if (match(TokenType::Star)) {
        stmt->projection_fields.push_back("*");
    } else {
        while (!check(TokenType::From) && !check(TokenType::Eof)) {
            if (match(TokenType::Identifier)) {
                stmt->projection_fields.push_back(tokens_[current_ - 1].text);
            }
            if (!match(TokenType::Comma)) break;
        }
    }

    if (match(TokenType::From)) {
        if (match(TokenType::Identifier)) {
            stmt->from_table = tokens_[current_ - 1].text;
        }
    }

    if (match(TokenType::Where)) {
        stmt->where_clause = parse_expression();
    }

    if (match(TokenType::Order)) {
        if (match(TokenType::By)) {
            if (match(TokenType::Identifier)) {
                stmt->order_by_field = tokens_[current_ - 1].text;
            }
            if (match(TokenType::Desc)) {
                stmt->order_desc = true;
            } else if (match(TokenType::Asc)) {
                stmt->order_desc = false;
            }
        }
    }

    if (match(TokenType::Limit)) {
        if (match(TokenType::NumberLiteral)) {
            stmt->limit = static_cast<size_t>(tokens_[current_ - 1].int_val);
        }
    }

    return Result<std::unique_ptr<SelectStatement>>::ok(std::move(stmt));
}

}
