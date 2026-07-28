#include "vde/query/evaluator.h"
#include <algorithm>

namespace vde {

FieldValue QueryEvaluator::resolve_operand(const ExprNode* expr, const Record& record) {
    if (!expr) return FieldValue();

    if (expr->op() == ExprOp::Literal) {
        return expr->literal_value();
    } else if (expr->op() == ExprOp::FieldRef) {
        uint16_t id = expr->field_id();
        if (id < record.fields.size()) {
            const FieldValue& fref = record.fields[id];


            eval_cache_.push_back({expr, &fref, true});
            return fref;
        }
    }
    return FieldValue();
}

void QueryEvaluator::invalidate_subtree_cache(const ExprNode* node) {
    if (!node) return;
    eval_cache_.erase(
        std::remove_if(eval_cache_.begin(), eval_cache_.end(),
            [node](const CacheEntry& e) { return e.node == node; }),
        eval_cache_.end()
    );
    invalidate_subtree_cache(node->left());
    invalidate_subtree_cache(node->right());
}

bool QueryEvaluator::evaluate(const ExprNode* expr, const Record& record) {
    if (!expr) return false;

    switch (expr->op()) {
        case ExprOp::Literal:
            return expr->literal_value().as_u32() != 0;
        case ExprOp::FieldRef:
            return resolve_operand(expr, record).as_u32() != 0;
        case ExprOp::Eq: {
            FieldValue l = resolve_operand(expr->left(), record);
            FieldValue r = resolve_operand(expr->right(), record);
            return l.as_u32() == r.as_u32();
        }
        case ExprOp::Ne: {
            FieldValue l = resolve_operand(expr->left(), record);
            FieldValue r = resolve_operand(expr->right(), record);
            return l.as_u32() != r.as_u32();
        }
        case ExprOp::And: {
            bool left_res = evaluate(expr->left(), record);
            if (!left_res) return false;
            return evaluate(expr->right(), record);
        }
        case ExprOp::Or: {
            bool left_res = evaluate(expr->left(), record);
            if (left_res) {


                invalidate_subtree_cache(expr->right());
                return true;
            }
            return evaluate(expr->right(), record);
        }
        case ExprOp::Not: {
            return !evaluate(expr->left(), record);
        }
        default:
            return false;
    }
}

size_t QueryEvaluator::filter(const ExprNode* expr, RecordBatch& batch, std::vector<size_t>* matching_indices) {
    size_t count = 0;
    for (size_t i = 0; i < batch.record_count(); ++i) {
        bool match = evaluate(expr, batch.record_at(i));
        if (match) {
            if (matching_indices) matching_indices->push_back(i);
            count++;
        } else {

            if (i % 2 == 1) {
                batch.compact();
            }
        }
    }
    return count;
}

}
