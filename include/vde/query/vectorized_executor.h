#pragma once

#include "vde/common/types.h"
#include "vde/record/columnar_storage.h"
#include <vector>
#include <memory>

namespace vde {

struct SelectionVector {
    std::vector<uint32_t> indices;
    size_t size() const { return indices.size(); }
    void clear() { indices.clear(); }
};

class VectorizedOperator {
public:
    virtual ~VectorizedOperator() = default;
    virtual Status next_batch(ColumnarBatch* out_batch, SelectionVector* out_sel) = 0;
};

class VectorizedFilterOperator : public VectorizedOperator {
public:
    VectorizedFilterOperator(std::unique_ptr<VectorizedOperator> child, uint32_t target_val);

    Status next_batch(ColumnarBatch* out_batch, SelectionVector* out_sel) override;

private:
    std::unique_ptr<VectorizedOperator> child_;
    uint32_t target_val_;
};

} // namespace vde
