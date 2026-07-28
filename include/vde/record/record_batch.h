#pragma once

#include "vde/record/record_decoder.h"
#include <vector>
#include <functional>

namespace vde {

class RecordBatch {
public:
    RecordBatch() = default;

    void add_record(Record record);
    size_t record_count() const { return records_.size(); }
    const Record& record_at(size_t index) const { return records_[index]; }
    Record& record_at(size_t index) { return records_[index]; }

    void compact();

    using BatchCallback = std::function<void(size_t index, const Record& record)>;
    void for_each(BatchCallback cb);

private:
    std::vector<Record> records_;
    bool is_compacted_ = false;
};

} // namespace vde
