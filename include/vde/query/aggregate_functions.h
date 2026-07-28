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
    explicit AggregateFunction(AggregateType type = AggregateType::Count, uint16_t field_id = 0);

    void update(const Record& record);
    void update(const FieldValue& val);
    FieldValue result() const;
    FieldValue evaluate() const { return result(); }
    void reset();

private:
    AggregateType type_;
    uint16_t field_id_;
    size_t count_ = 0;
    double sum_ = 0.0;
    double min_val_ = 1e300;
    double max_val_ = -1e300;
    double M2_ = 0.0;
};

}
