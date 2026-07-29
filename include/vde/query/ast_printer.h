#pragma once

#include "vde/query/filter_ast.h"
#include <string>

namespace vde {

class AstPrinter : public AstVisitor {
public:
    AstPrinter() = default;

    std::string print(FilterAstNode* node);

    void visit(ChannelRefNode* node) override;
    void visit(ColumnRefNode* node) override;
    void visit(LiteralNode* node) override;
    void visit(BinaryOpNode* node) override;
    void visit(FilterQueryNode* node) override;

private:
    std::string result_;
    int indent_ = 0;
};

}
