#pragma once

#include "vde/query/sql_ast.h"
#include <vector>
#include <cstdint>

namespace vde {

struct HistogramBucket {
    double min_val;
    double max_val;
    uint64_t count;
};

class EquiHeightHistogram {
public:
    EquiHeightHistogram() = default;

    void build(const std::vector<double>& sample_values, size_t bucket_count = 10);
    double estimate_selectivity_eq(double val) const;
    double estimate_selectivity_range(double min_val, double max_val) const;

    size_t bucket_count() const { return buckets_.size(); }
    uint64_t total_samples() const { return total_samples_; }

private:
    std::vector<HistogramBucket> buckets_;
    uint64_t total_samples_ = 0;
};

class CostModel {
public:
    CostModel() = default;

    double estimate_scan_cost(uint64_t total_pages, uint64_t total_tuples) const;
    double estimate_index_scan_cost(uint64_t index_height, double selectivity, uint64_t total_tuples) const;
    double estimate_hash_join_cost(uint64_t left_tuples, uint64_t right_tuples) const;
    double estimate_nested_loop_join_cost(uint64_t outer_tuples, uint64_t inner_tuples) const;
};

} // namespace vde
