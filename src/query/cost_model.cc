#include "vde/query/cost_model.h"
#include <algorithm>
#include <cmath>

namespace vde {

void EquiHeightHistogram::build(const std::vector<double>& sample_values, size_t bucket_count) {
    buckets_.clear();
    total_samples_ = sample_values.size();
    if (sample_values.empty() || bucket_count == 0) return;

    std::vector<double> sorted = sample_values;
    std::sort(sorted.begin(), sorted.end());

    size_t samples_per_bucket = sorted.size() / bucket_count;
    if (samples_per_bucket == 0) samples_per_bucket = 1;

    for (size_t i = 0; i < sorted.size(); i += samples_per_bucket) {
        size_t end_idx = std::min(i + samples_per_bucket - 1, sorted.size() - 1);
        buckets_.push_back({sorted[i], sorted[end_idx], end_idx - i + 1});
    }
}

double EquiHeightHistogram::estimate_selectivity_eq(double val) const {
    if (total_samples_ == 0 || buckets_.empty()) return 0.05;
    for (const auto& b : buckets_) {
        if (val >= b.min_val && val <= b.max_val) {
            double range = (b.max_val - b.min_val);
            if (range < 1e-9) return static_cast<double>(b.count) / total_samples_;
            return (1.0 / range) * (static_cast<double>(b.count) / total_samples_);
        }
    }
    return 0.01;
}

double EquiHeightHistogram::estimate_selectivity_range(double min_val, double max_val) const {
    if (total_samples_ == 0 || buckets_.empty()) return 0.25;
    uint64_t matched_count = 0;

    for (const auto& b : buckets_) {
        if (b.max_val >= min_val && b.min_val <= max_val) {
            matched_count += b.count;
        }
    }

    return static_cast<double>(matched_count) / total_samples_;
}

double CostModel::estimate_scan_cost(uint64_t total_pages, uint64_t total_tuples) const {
    return total_pages * 1.0 + total_tuples * 0.01;
}

double CostModel::estimate_index_scan_cost(uint64_t index_height, double selectivity, uint64_t total_tuples) const {
    return index_height * 0.5 + (selectivity * total_tuples) * 1.1;
}

double CostModel::estimate_hash_join_cost(uint64_t left_tuples, uint64_t right_tuples) const {
    return (left_tuples * 1.2) + (right_tuples * 0.8);
}

double CostModel::estimate_nested_loop_join_cost(uint64_t outer_tuples, uint64_t inner_tuples) const {
    return outer_tuples * inner_tuples * 0.05;
}

}
