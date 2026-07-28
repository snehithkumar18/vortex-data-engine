#include "vde/query/expression_interpreter.h"

namespace vde {

Result<FieldValue> ExpressionInterpreter::evaluate_node(const ExprNode* node, const Record& record) {
    if (!node) return Result<FieldValue>::error(Status::InvalidArgument);

    if (node->op() == ExprOp::Literal) {
        return Result<FieldValue>::ok(node->literal_value());
    } else if (node->op() == ExprOp::FieldRef) {
        if (node->field_id() < record.fields.size()) {
            return Result<FieldValue>::ok(record.fields[node->field_id()]);
        }
        return Result<FieldValue>::ok(FieldValue(uint32_t(0)));
    }

    return Result<FieldValue>::ok(FieldValue(uint32_t(0)));
}

std::vector<bool> ExpressionInterpreter::evaluate_batch(const ExprNode* node, const RecordBatch& batch) {
    std::vector<bool> res(batch.record_count(), false);
    for (size_t i = 0; i < batch.record_count(); ++i) {
        auto val_res = evaluate_node(node, batch.record_at(i));
        if (val_res.has_value()) {
            res[i] = val_res.value.as_u32() != 0;
        }
    }
    return res;
}

} // namespace vde
