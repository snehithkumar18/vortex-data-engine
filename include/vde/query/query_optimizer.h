#pragma once

#include "vde/query/sql_parser.h"

namespace vde {

class QueryOptimizer {
public:
    QueryOptimizer() = default;

    Status optimize(SelectStatement* stmt);

private:
    bool fold_constants(ExprNode* expr);
    bool pushdown_predicates(SelectStatement* stmt);
};

}
