#include "vde/query/query_optimizer.h"

namespace vde {

Status QueryOptimizer::optimize(SelectStatement* stmt) {
    if (!stmt) return Status::InvalidArgument;
    if (stmt->where_clause) {
        fold_constants(stmt->where_clause);
        pushdown_predicates(stmt);
    }
    return Status::Ok;
}

bool QueryOptimizer::fold_constants(ExprNode* expr) {
    if (!expr) return false;
    fold_constants(expr->left());
    fold_constants(expr->right());
    return true;
}

bool QueryOptimizer::pushdown_predicates(SelectStatement* stmt) {
    (void)stmt;
    return true;
}

}
