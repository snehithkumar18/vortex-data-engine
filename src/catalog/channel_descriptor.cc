#include "vde/telemetry_registry/channel_descriptor.h"

namespace vde {

ChannelDescriptor::ChannelDescriptor(std::string name, Schema schema)
    : name_(std::move(name)), schema_(std::move(schema)) {}

void ChannelDescriptor::add_column_metadata(ColumnMetadata col) {
    columns_.push_back(std::move(col));
}

const ColumnMetadata* ChannelDescriptor::find_column(const std::string& col_name) const {
    for (const auto& col : columns_) {
        if (col.name == col_name) return &col;
    }
    return nullptr;
}

}
