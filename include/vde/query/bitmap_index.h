#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdint>

namespace vde {

class BitmapIndex {
public:
    explicit BitmapIndex(size_t total_records = 0);
    ~BitmapIndex() = default;

    void add_value(uint32_t category, size_t record_index);
    std::vector<size_t> query_category(uint32_t category) const;

private:
    struct CategoryBitmap {
        uint32_t category;
        std::vector<uint64_t> bitmap;
    };

    size_t total_records_;
    std::vector<CategoryBitmap> categories_;
};

} // namespace vde
