#pragma once

#include "vde/common/types.h"
#include "vde/record/schema.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace vde {

struct ColumnMetadata {
    std::string name;
    FieldType type;
    uint32_t length = 0;
    bool is_nullable = true;
    bool is_primary_key = false;
    bool is_indexed = false;
    std::string default_value;
};

class TableDescriptor {
public:
    TableDescriptor(std::string name, Schema schema);

    const std::string& name() const { return name_; }
    const Schema& schema() const { return schema_; }

    void add_column_metadata(ColumnMetadata col);
    const ColumnMetadata* find_column(const std::string& col_name) const;

    size_t column_count() const { return columns_.size(); }
    const std::vector<ColumnMetadata>& columns() const { return columns_; }

    void set_record_count(uint64_t count) { record_count_ = count; }
    uint64_t record_count() const { return record_count_; }

    void set_data_file_path(std::string path) { data_file_path_ = std::move(path); }
    const std::string& data_file_path() const { return data_file_path_; }

private:
    std::string name_;
    Schema schema_;
    std::vector<ColumnMetadata> columns_;
    uint64_t record_count_ = 0;
    std::string data_file_path_;
};

}
