#pragma once

#include "vde/common/types.h"
#include <vector>
#include <memory>
#include <cmath>

namespace vde {

struct BoundingBox {
    double min_x = 0.0;
    double min_y = 0.0;
    double max_x = 0.0;
    double max_y = 0.0;

    bool intersects(const BoundingBox& other) const {
        return !(min_x > other.max_x || max_x < other.min_x ||
                 min_y > other.max_y || max_y < other.min_y);
    }

    double area() const {
        return (max_x - min_x) * (max_y - min_y);
    }
};

struct SpatialRegionNode {
    bool is_leaf = true;
    BoundingBox bbox;
    std::vector<size_t> record_indices;
    std::vector<BoundingBox> child_boxes;
    std::vector<std::unique_ptr<SpatialRegionNode>> children;
};

class SpatialRegionIndex {
public:
    explicit SpatialRegionIndex(size_t max_entries = 8);
    ~SpatialRegionIndex() = default;

    void insert(const BoundingBox& box, size_t record_index);
    std::vector<size_t> query_range(const BoundingBox& range) const;

    size_t total_items() const { return total_items_; }

private:
    void query_recursive(const SpatialRegionNode* node, const BoundingBox& range, std::vector<size_t>& results) const;
    BoundingBox compute_bounds(const std::vector<BoundingBox>& boxes) const;

    std::unique_ptr<SpatialRegionNode> root_;
    size_t max_entries_;
    size_t total_items_ = 0;
};

}
