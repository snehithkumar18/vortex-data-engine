#include "vde/query/ast_printer.h"

namespace vde {

std::string AstPrinter::print(SqlAstNode* node) {
    result_.clear();
    indent_ = 0;
    if (node) node->accept(this);
    return result_;
}

void AstPrinter::visit(TableRefNode* node) {
    result_ += "TableRef(" + node->table_name() + ")";
}

void AstPrinter::visit(ColumnRefNode* node) {
    result_ += "ColumnRef(" + node->column_name() + ")";
}

void AstPrinter::visit(LiteralNode* node) {
    (void)node;
    result_ += "Literal(...)";
}

void AstPrinter::visit(BinaryOpNode* node) {
    result_ += "BinaryOp(";
    if (node->left()) node->left()->accept(this);
    result_ += ", ";
    if (node->right()) node->right()->accept(this);
    result_ += ")";
}

void AstPrinter::visit(SelectQueryNode* node) {
    result_ += "SelectQueryNode[\n";
    if (node->from_table()) {
        result_ += "  FROM: ";
        node->from_table()->accept(this);
        result_ += "\n";
    }
    if (node->where_clause()) {
        result_ += "  WHERE: ";
        node->where_clause()->accept(this);
        result_ += "\n";
    }
    result_ += "]";
}

}
