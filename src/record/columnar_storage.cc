#include "vde/record/columnar_storage.h"

namespace vde {

void ColumnarBatch::add_column(const std::string& name, std::unique_ptr<ColumnVector> col) {
    if (col) {
        row_count_ = col->size();
        names_.push_back(name);
        columns_.push_back(std::move(col));
    }
}

const ColumnVector* ColumnarBatch::get_column(const std::string& name) const {
    for (size_t i = 0; i < names_.size(); ++i) {
        if (names_[i] == name) return columns_[i].get();
    }
    return nullptr;
}

ColumnarBatch ColumnarBatch::from_record_batch(const RecordBatch& batch) {
    ColumnarBatch cbatch;
    if (batch.record_count() == 0) return cbatch;

    auto col0 = std::make_unique<Uint32ColumnVector>();
    for (size_t i = 0; i < batch.record_count(); ++i) {
        const auto& rec = batch.record_at(i);
        if (!rec.fields.empty()) {
            col0->add(rec.fields[0].as_u32());
        } else {
            col0->add(0);
        }
    }
    cbatch.add_column("field0", std::move(col0));
    return cbatch;
}

}
