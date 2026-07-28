#include "vde/record/arrow_bridge.h"

namespace vde {

Status ArrowDataBridge::export_column(const ColumnVector& col, ArrowArrayBridge* out_array, ArrowSchemaBridge* out_schema) {
    if (!out_array || !out_schema) return Status::InvalidArgument;

    out_array->length = static_cast<int64_t>(col.size());
    out_array->null_count = 0;
    out_array->buffers[0] = nullptr;
    out_array->buffers[1] = col.raw_data();
    out_array->buffers[2] = nullptr;

    if (col.type() == FieldType::Uint32) {
        out_schema->format = "I";
    } else if (col.type() == FieldType::Int64) {
        out_schema->format = "l";
    } else {
        out_schema->format = "z";
    }

    out_schema->name = "vde_col";
    out_schema->flags = 0;
    return Status::Ok;
}

}
