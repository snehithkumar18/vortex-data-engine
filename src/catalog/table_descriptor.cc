#include "vde/catalog/table_descriptor.h"

namespace vde {

TableDescriptor::TableDescriptor(std::string name, Schema schema)
    : name_(std::move(name)), schema_(std::move(schema)) {}

void TableDescriptor::add_column_metadata(ColumnMetadata col) {
    columns_.push_back(std::move(col));
}

const ColumnMetadata* TableDescriptor::find_column(const std::string& col_name) const {
    for (const auto& col : columns_) {
        if (col.name == col_name) return &col;
    }
    return nullptr;
}

}
