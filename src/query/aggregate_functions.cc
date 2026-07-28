#include "vde/query/aggregate_functions.h"

namespace vde {

AggregateFunction::AggregateFunction(AggregateType type, uint16_t field_id)
    : type_(type), field_id_(field_id) {}

void AggregateFunction::reset() {
    count_ = 0;
    sum_ = 0.0;
    min_val_ = 1e300;
    max_val_ = -1e300;
    M2_ = 0.0;
}

void AggregateFunction::update(const Record& record) {
    count_++;
    if (field_id_ < record.fields.size()) {
        const auto& val = record.fields[field_id_];
        double num = 0.0;
        if (val.type() == FieldType::Uint32) num = val.as_u32();
        else if (val.type() == FieldType::Int64) num = static_cast<double>(val.as_i64());
        else if (val.type() == FieldType::Float64) num = val.as_f64();
        else return;

        sum_ += num;
        if (num < min_val_) min_val_ = num;
        if (num > max_val_) max_val_ = num;

        // Welford's variance calculation
        double delta = num - (sum_ / count_);
        double mean_new = sum_ / count_;
        double delta2 = num - mean_new;
        M2_ += delta * delta2;
    }
}

FieldValue AggregateFunction::result() const {
    switch (type_) {
        case AggregateType::Count:
            return FieldValue(static_cast<uint32_t>(count_));
        case AggregateType::Sum:
            return FieldValue(sum_);
        case AggregateType::Avg:
            return FieldValue(count_ > 0 ? sum_ / count_ : 0.0);
        case AggregateType::Min:
            return FieldValue(min_val_ < 1e299 ? min_val_ : 0.0);
        case AggregateType::Max:
            return FieldValue(max_val_ > -1e299 ? max_val_ : 0.0);
        case AggregateType::StdDev: {
            if (count_ < 2) return FieldValue(0.0);
            double variance = M2_ / (count_ - 1);
            return FieldValue(std::sqrt(variance));
        }
    }
    return FieldValue();
}

} // namespace vde
