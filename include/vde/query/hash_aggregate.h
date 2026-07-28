#pragma once

#include "vde/query/query_planner.h"
#include "vde/query/aggregate_functions.h"
#include <unordered_map>
#include <memory>

namespace vde {

class HashAggregateOperator : public PhysicalOperator {
public:
    HashAggregateOperator(std::unique_ptr<PhysicalOperator> child,
                          uint16_t group_by_field,
                          uint16_t aggregate_field,
                          AggregateType agg_type);

    Status open() override;
    Result<Record> next() override;
    void close() override;

private:
    std::unique_ptr<PhysicalOperator> child_;
    uint16_t group_by_field_;
    uint16_t aggregate_field_;
    AggregateType agg_type_;

    struct GroupState {
        uint32_t group_key;
        AggregateFunction agg_fn;
    };

    std::unordered_map<uint32_t, AggregateFunction> groups_;
    std::vector<std::pair<uint32_t, FieldValue>> results_;
    size_t result_idx_ = 0;
};

} // namespace vde
