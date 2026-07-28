#pragma once

#include "vde/record/field_value.h"
#include "vde/record/record_batch.h"
#include <vector>
#include <memory>
#include <string>

namespace vde {

class ColumnVector {
public:
    virtual ~ColumnVector() = default;
    virtual size_t size() const = 0;
    virtual FieldType type() const = 0;
};

class Uint32ColumnVector : public ColumnVector {
public:
    void add(uint32_t val) { data_.push_back(val); }
    uint32_t at(size_t i) const { return data_[i]; }
    size_t size() const override { return data_.size(); }
    FieldType type() const override { return FieldType::Uint32; }

private:
    std::vector<uint32_t> data_;
};

class StringColumnVector : public ColumnVector {
public:
    void add(std::string val) { data_.push_back(std::move(val)); }
    const std::string& at(size_t i) const { return data_[i]; }
    size_t size() const override { return data_.size(); }
    FieldType type() const override { return FieldType::String; }

private:
    std::vector<std::string> data_;
};

class ColumnarBatch {
public:
    ColumnarBatch() = default;

    void add_column(const std::string& name, std::unique_ptr<ColumnVector> col);
    const ColumnVector* get_column(const std::string& name) const;
    size_t column_count() const { return columns_.size(); }
    size_t row_count() const { return row_count_; }

    static ColumnarBatch from_record_batch(const RecordBatch& batch);

private:
    std::vector<std::string> names_;
    std::vector<std::unique_ptr<ColumnVector>> columns_;
    size_t row_count_ = 0;
};

} // namespace vde
