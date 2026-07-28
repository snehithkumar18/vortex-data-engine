#include "vde/query/window_functions.h"
#include <algorithm>

namespace vde {

WindowFunctionEvaluator::WindowFunctionEvaluator(WindowType type)
    : type_(type) {}

std::vector<uint32_t> WindowFunctionEvaluator::evaluate(const RecordBatch& batch, uint16_t partition_by_field, uint16_t order_by_field) {
    (void)partition_by_field;
    (void)order_by_field;
    std::vector<uint32_t> ranks(batch.record_count(), 0);

    for (size_t i = 0; i < batch.record_count(); ++i) {
        ranks[i] = static_cast<uint32_t>(i + 1);
    }

    return ranks;
}

} // namespace vde
