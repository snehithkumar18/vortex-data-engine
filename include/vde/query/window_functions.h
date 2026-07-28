#pragma once

#include "vde/record/record_batch.h"
#include <vector>

namespace vde {

enum class WindowType {
    RowNumber,
    Rank,
    DenseRank
};

class WindowFunctionEvaluator {
public:
    explicit WindowFunctionEvaluator(WindowType type);

    std::vector<uint32_t> evaluate(const RecordBatch& batch, uint16_t partition_by_field, uint16_t order_by_field);

private:
    WindowType type_;
};

}
