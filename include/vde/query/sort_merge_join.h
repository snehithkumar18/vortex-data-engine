#pragma once

#include "vde/query/query_planner.h"
#include <memory>
#include <vector>

namespace vde {

class SortMergeJoinOperator : public PhysicalOperator {
public:
    SortMergeJoinOperator(std::unique_ptr<PhysicalOperator> left_child,
                           std::unique_ptr<PhysicalOperator> right_child,
                           uint16_t left_key_idx,
                           uint16_t right_key_idx);

    Status open() override;
    Result<Record> next() override;
    void close() override;

private:
    std::unique_ptr<PhysicalOperator> left_child_;
    std::unique_ptr<PhysicalOperator> right_child_;
    uint16_t left_key_idx_;
    uint16_t right_key_idx_;

    std::vector<Record> sorted_left_;
    std::vector<Record> sorted_right_;
    size_t left_pos_ = 0;
    size_t right_pos_ = 0;
};

} // namespace vde
