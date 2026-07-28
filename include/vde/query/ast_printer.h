#pragma once

#include "vde/query/sql_ast.h"
#include <string>

namespace vde {

class AstPrinter : public AstVisitor {
public:
    AstPrinter() = default;

    std::string print(SqlAstNode* node);

    void visit(TableRefNode* node) override;
    void visit(ColumnRefNode* node) override;
    void visit(LiteralNode* node) override;
    void visit(BinaryOpNode* node) override;
    void visit(SelectQueryNode* node) override;

private:
    std::string result_;
    int indent_ = 0;
};

}
