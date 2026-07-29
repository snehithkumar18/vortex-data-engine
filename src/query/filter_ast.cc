#include "vde/query/filter_ast.h"

namespace vde {

void ChannelRefNode::accept(AstVisitor* visitor) {
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

void FilterQueryNode::accept(AstVisitor* visitor) {
    if (visitor) visitor->visit(this);
}

void FilterQueryNode::add_projection(std::unique_ptr<FilterAstNode> proj, std::string alias) {
    projections_.push_back(std::move(proj));
    aliases_.push_back(std::move(alias));
}

void FilterQueryNode::set_from_table(std::unique_ptr<ChannelRefNode> table) {
    from_table_ = std::move(table);
}

void FilterQueryNode::set_where_clause(std::unique_ptr<FilterAstNode> where) {
    where_clause_ = std::move(where);
}

}
