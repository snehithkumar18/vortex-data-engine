#pragma once

#include "vde/query/expression.h"
#include "vde/record/record_batch.h"
#include <vector>

namespace vde {

class QueryEvaluator {
public:
    QueryEvaluator() = default;

    bool evaluate(const ExprNode* expr, const Record& record);
    size_t filter(const ExprNode* expr, RecordBatch& batch, std::vector<size_t>* matching_indices);

private:
    struct CacheEntry {
        const ExprNode* node;
        const FieldValue* val_ref;
        bool result_bool;
    };

    FieldValue resolve_operand(const ExprNode* expr, const Record& record);
    void invalidate_subtree_cache(const ExprNode* node);

    std::vector<CacheEntry> eval_cache_;
};

} // namespace vde
