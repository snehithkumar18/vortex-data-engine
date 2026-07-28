#pragma once

#include "vde/common/types.h"
#include "vde/query/query_planner.h"
#include <memory>

namespace vde {

class HashJoinOperator : public PhysicalOperator {
public:
    HashJoinOperator(std::unique_ptr<PhysicalOperator> left_child,
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

    struct HashBucket {
        uint32_t key;
        Record record;
    };
    std::vector<HashBucket> hashtable_;
    size_t current_match_idx_ = 0;
};

} // namespace vde
