#include "vde/query/spatial_region_index.h"
#include <algorithm>

namespace vde {

SpatialRegionIndex::SpatialRegionIndex(size_t max_entries)
    : root_(std::make_unique<SpatialRegionNode>()), max_entries_(max_entries < 4 ? 4 : max_entries) {}

BoundingBox SpatialRegionIndex::compute_bounds(const std::vector<BoundingBox>& boxes) const {
    if (boxes.empty()) return BoundingBox();
    BoundingBox b = boxes.front();
    for (const auto& box : boxes) {
        if (box.min_x < b.min_x) b.min_x = box.min_x;
        if (box.min_y < b.min_y) b.min_y = box.min_y;
        if (box.max_x > b.max_x) b.max_x = box.max_x;
        if (box.max_y > b.max_y) b.max_y = box.max_y;
    }
    return b;
}

void SpatialRegionIndex::insert(const BoundingBox& box, size_t record_index) {
    total_items_++;
    root_->child_boxes.push_back(box);
    root_->record_indices.push_back(record_index);
    root_->bbox = compute_bounds(root_->child_boxes);
}

std::vector<size_t> SpatialRegionIndex::query_range(const BoundingBox& range) const {
    std::vector<size_t> results;
    query_recursive(root_.get(), range, results);
    return results;
}

void SpatialRegionIndex::query_recursive(const SpatialRegionNode* node, const BoundingBox& range, std::vector<size_t>& results) const {
    if (!node || !node->bbox.intersects(range)) return;

    if (node->is_leaf) {
        for (size_t i = 0; i < node->child_boxes.size(); ++i) {
            if (node->child_boxes[i].intersects(range)) {
                results.push_back(node->record_indices[i]);
            }
        }
    } else {
        for (const auto& child : node->children) {
            query_recursive(child.get(), range, results);
        }
    }
}

}
