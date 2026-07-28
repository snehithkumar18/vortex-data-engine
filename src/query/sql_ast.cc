#include "vde/query/sql_ast.h"

namespace vde {

void TableRefNode::accept(AstVisitor* visitor) {
    if (visitor) visitor->visit(this);
}

void ColumnRefNode::accept(AstVisitor* visitor) {
    if (visitor) visitor->visit(this);
}

void LiteralNode::accept(AstVisitor* visitor) {
    if (visitor) visitor->visit(this);
}

void BinaryOpNode::accept(AstVisitor* visitor) {
    if (visitor) visitor->visit(this);
}

void SelectQueryNode::accept(AstVisitor* visitor) {
    if (visitor) visitor->visit(this);
}

void SelectQueryNode::add_projection(std::unique_ptr<SqlAstNode> proj, std::string alias) {
    projections_.push_back(std::move(proj));
    aliases_.push_back(std::move(alias));
}

void SelectQueryNode::set_from_table(std::unique_ptr<TableRefNode> table) {
    from_table_ = std::move(table);
}

void SelectQueryNode::set_where_clause(std::unique_ptr<SqlAstNode> where) {
    where_clause_ = std::move(where);
}

} // namespace vde
