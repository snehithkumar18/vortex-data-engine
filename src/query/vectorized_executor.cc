#include "vde/query/vectorized_executor.h"

namespace vde {

VectorizedFilterOperator::VectorizedFilterOperator(std::unique_ptr<VectorizedOperator> child, uint32_t target_val)
    : child_(std::move(child)), target_val_(target_val) {}

Status VectorizedFilterOperator::next_batch(ColumnarBatch* out_batch, SelectionVector* out_sel) {
    if (!child_ || !out_batch || !out_sel) return Status::InvalidArgument;

    SelectionVector child_sel;
    Status st = child_->next_batch(out_batch, &child_sel);
    if (st != Status::Ok) return st;

    out_sel->clear();
    const auto* col = out_batch->get_column("field0");
    if (!col) return Status::Ok;

    const auto* u32_col = dynamic_cast<const Uint32ColumnVector*>(col);
    if (!u32_col) return Status::Ok;

    // Vectorized evaluation loop
    for (size_t i = 0; i < u32_col->size(); ++i) {
        if (u32_col->at(i) == target_val_) {
            out_sel->indices.push_back(static_cast<uint32_t>(i));
        }
    }

    return Status::Ok;
}

} // namespace vde
