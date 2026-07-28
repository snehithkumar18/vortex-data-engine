#pragma once

#include "vde/record/columnar_storage.h"

namespace vde {

struct ArrowArrayBridge {
    int64_t length;
    int64_t null_count;
    const void* buffers[3];
};

struct ArrowSchemaBridge {
    const char* format;
    const char* name;
    int64_t flags;
};

class ArrowDataBridge {
public:
    ArrowDataBridge() = default;

    static Status export_column(const ColumnVector& col, ArrowArrayBridge* out_array, ArrowSchemaBridge* out_schema);
};

}
