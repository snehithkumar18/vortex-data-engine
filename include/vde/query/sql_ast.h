#pragma once

#include "vde/common/types.h"
#include "vde/record/field_value.h"
#include <string>
#include <vector>
#include <memory>

namespace vde {

class AstVisitor;

class SqlAstNode {
public:
    virtual ~SqlAstNode() = default;
    virtual void accept(AstVisitor* visitor) = 0;
};

class TableRefNode : public SqlAstNode {
public:
    explicit TableRefNode(std::string table_name, std::string alias = "")
        : table_name_(std::move(table_name)), alias_(std::move(alias)) {}

    void accept(AstVisitor* visitor) override;

    const std::string& table_name() const { return table_name_; }
    const std::string& alias() const { return alias_; }

private:
    std::string table_name_;
    std::string alias_;
};

class ColumnRefNode : public SqlAstNode {
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

class LiteralNode : public SqlAstNode {
public:
    explicit LiteralNode(FieldValue value) : value_(std::move(value)) {}

    void accept(AstVisitor* visitor) override;

    const FieldValue& value() const { return value_; }

private:
    FieldValue value_;
};

class BinaryOpNode : public SqlAstNode {
public:
    enum class Op { Add, Sub, Mul, Div, Mod, Equal, NotEqual, Less, Greater, LessEq, GreaterEq, And, Or };

    BinaryOpNode(Op op, std::unique_ptr<SqlAstNode> left, std::unique_ptr<SqlAstNode> right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}

    void accept(AstVisitor* visitor) override;

    Op op() const { return op_; }
    SqlAstNode* left() const { return left_.get(); }
    SqlAstNode* right() const { return right_.get(); }

private:
    Op op_;
    std::unique_ptr<SqlAstNode> left_;
    std::unique_ptr<SqlAstNode> right_;
};

class SelectQueryNode : public SqlAstNode {
public:
    SelectQueryNode() = default;

    void accept(AstVisitor* visitor) override;

    void add_projection(std::unique_ptr<SqlAstNode> proj, std::string alias = "");
    void set_from_table(std::unique_ptr<TableRefNode> table);
    void set_where_clause(std::unique_ptr<SqlAstNode> where);

    const std::vector<std::unique_ptr<SqlAstNode>>& projections() const { return projections_; }
    const TableRefNode* from_table() const { return from_table_.get(); }
    const SqlAstNode* where_clause() const { return where_clause_.get(); }

private:
    std::vector<std::unique_ptr<SqlAstNode>> projections_;
    std::vector<std::string> aliases_;
    std::unique_ptr<TableRefNode> from_table_;
    std::unique_ptr<SqlAstNode> where_clause_;
};

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(TableRefNode* node) = 0;
    virtual void visit(ColumnRefNode* node) = 0;
    virtual void visit(LiteralNode* node) = 0;
    virtual void visit(BinaryOpNode* node) = 0;
    virtual void visit(SelectQueryNode* node) = 0;
};

}
