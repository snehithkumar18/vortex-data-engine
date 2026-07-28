#pragma once

#include "vde/common/types.h"
#include "vde/record/field_value.h"
#include "vde/record/record_batch.h"
#include <cmath>

namespace vde {

enum class AggregateType {
    Count,
    Sum,
    Avg,
    Min,
    Max,
    StdDev
};

class AggregateFunction {
public:
    explicit AggregateFunction(AggregateType type, uint16_t field_id = 0);

    void update(const Record& record);
    FieldValue result() const;
    void reset();

private:
    AggregateType type_;
    uint16_t field_id_;
    size_t count_ = 0;
    double sum_ = 0.0;
    double min_val_ = 1e300;
    double max_val_ = -1e300;
    double M2_ = 0.0; // Welford's algorithm variance tracker
};

} // namespace vde
