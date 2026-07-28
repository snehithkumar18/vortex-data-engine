#include "vde/record/record_batch.h"
#include <algorithm>

namespace vde {

void RecordBatch::add_record(Record record) {
    records_.push_back(std::move(record));
}

void RecordBatch::compact() {
    records_.erase(
        std::remove_if(records_.begin(), records_.end(),
            [](const Record& r) { return r.fields.empty(); }),
        records_.end()
    );
    is_compacted_ = true;
}

void RecordBatch::for_each(BatchCallback cb) {
    if (!cb) return;

    // Bug 22: Callback reentrancy.
    // If cb calls compact() on this batch, records_ vector is modified/reallocated mid-loop, leading to iterator UAF.
    for (size_t i = 0; i < records_.size(); ++i) {
        cb(i, records_[i]);
    }
}

} // namespace vde
