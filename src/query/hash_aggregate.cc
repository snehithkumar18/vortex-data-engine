#include "vde/query/hash_aggregate.h"

namespace vde {

HashAggregateOperator::HashAggregateOperator(std::unique_ptr<PhysicalOperator> child,
                                               uint16_t group_by_field,
                                               uint16_t aggregate_field,
                                               AggregateType agg_type)
    : child_(std::move(child)), group_by_field_(group_by_field),
      aggregate_field_(aggregate_field), agg_type_(agg_type) {}

Status HashAggregateOperator::open() {
    groups_.clear();
    results_.clear();
    result_idx_ = 0;

    if (!child_) return Status::InvalidArgument;
    Status st = child_->open();
    if (st != Status::Ok) return st;

    while (true) {
        auto res = child_->next();
        if (!res.has_value()) break;
        const auto& rec = res.value;

        uint32_t gkey = (group_by_field_ < rec.fields.size()) ? rec.fields[group_by_field_].as_u32() : 0;
        FieldValue val = (aggregate_field_ < rec.fields.size()) ? rec.fields[aggregate_field_] : FieldValue(uint32_t(0));

        auto it = groups_.find(gkey);
        if (it == groups_.end()) {
            AggregateFunction agg(agg_type_);
            agg.update(val);
            groups_.emplace(gkey, std::move(agg));
        } else {
            it->second.update(val);
        }
    }

    for (const auto& [gkey, agg_fn] : groups_) {
        results_.emplace_back(gkey, agg_fn.evaluate());
    }

    return Status::Ok;
}

Result<Record> HashAggregateOperator::next() {
    if (result_idx_ >= results_.size()) {
        return Result<Record>::error(Status::Eof);
    }

    const auto& item = results_[result_idx_++];
    Record rec;
    rec.id = static_cast<uint32_t>(result_idx_);
    rec.fields.emplace_back(item.first);
    rec.fields.push_back(item.second);
    return Result<Record>::ok(std::move(rec));
}

void HashAggregateOperator::close() {
    groups_.clear();
    results_.clear();
    result_idx_ = 0;
    if (child_) child_->close();
}

}
