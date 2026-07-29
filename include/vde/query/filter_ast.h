#pragma once

#include "vde/common/types.h"
#include "vde/record/field_value.h"
#include <string>
#include <vector>
#include <memory>

namespace vde {

class AstVisitor;

class FilterAstNode {
public:
    virtual ~FilterAstNode() = default;
    virtual void accept(AstVisitor* visitor) = 0;
};

class ChannelRefNode : public FilterAstNode {
public:
    explicit ChannelRefNode(std::string table_name, std::string alias = "")
        : table_name_(std::move(table_name)), alias_(std::move(alias)) {}

    void accept(AstVisitor* visitor) override;

    const std::string& table_name() const { return table_name_; }
    const std::string& alias() const { return alias_; }

private:
    std::string table_name_;
    std::string alias_;
};

class ColumnRefNode : public FilterAstNode {
public:
    explicit ColumnRefNode(std::string column_name, std::string table_qualifier = "")
        : column_name_(std::move(column_name)), table_qualifier_(std::move(table_qualifier)) {}

    void accept(AstVisitor* visitor) override;

    const std::string& column_name() const { return column_name_; }
    const std::string& table_qualifier() const { return table_qualifier_; }

private:
    std::string column_name_;
    std::string table_qualifier_;
};

class LiteralNode : public FilterAstNode {
public:
    explicit LiteralNode(FieldValue value) : value_(std::move(value)) {}

    void accept(AstVisitor* visitor) override;

    const FieldValue& value() const { return value_; }

private:
    FieldValue value_;
};

class BinaryOpNode : public FilterAstNode {
public:
    enum class Op { Add, Sub, Mul, Div, Mod, Equal, NotEqual, Less, Greater, LessEq, GreaterEq, And, Or };

    BinaryOpNode(Op op, std::unique_ptr<FilterAstNode> left, std::unique_ptr<FilterAstNode> right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}

    void accept(AstVisitor* visitor) override;

    Op op() const { return op_; }
    FilterAstNode* left() const { return left_.get(); }
    FilterAstNode* right() const { return right_.get(); }

private:
    Op op_;
    std::unique_ptr<FilterAstNode> left_;
    std::unique_ptr<FilterAstNode> right_;
};

class FilterQueryNode : public FilterAstNode {
public:
    FilterQueryNode() = default;

    void accept(AstVisitor* visitor) override;

    void add_projection(std::unique_ptr<FilterAstNode> proj, std::string alias = "");
    void set_from_table(std::unique_ptr<ChannelRefNode> table);
    void set_where_clause(std::unique_ptr<FilterAstNode> where);

    const std::vector<std::unique_ptr<FilterAstNode>>& projections() const { return projections_; }
    ChannelRefNode* from_table() const { return from_table_.get(); }
    FilterAstNode* where_clause() const { return where_clause_.get(); }

private:
    std::vector<std::unique_ptr<FilterAstNode>> projections_;
    std::vector<std::string> aliases_;
    std::unique_ptr<ChannelRefNode> from_table_;
    std::unique_ptr<FilterAstNode> where_clause_;
};

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ChannelRefNode* node) = 0;
    virtual void visit(ColumnRefNode* node) = 0;
    virtual void visit(LiteralNode* node) = 0;
    virtual void visit(BinaryOpNode* node) = 0;
    virtual void visit(FilterQueryNode* node) = 0;
};

}
