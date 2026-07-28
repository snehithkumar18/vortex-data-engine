#pragma once

#include "vde/query/expression.h"
#include "vde/record/record_batch.h"

namespace vde {

class ExpressionInterpreter {
public:
    ExpressionInterpreter() = default;

    Result<FieldValue> evaluate_node(const ExprNode* node, const Record& record);
    std::vector<bool> evaluate_batch(const ExprNode* node, const RecordBatch& batch);
};

}
