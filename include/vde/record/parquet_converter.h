#pragma once

#include "vde/record/columnar_storage.h"

namespace vde {

class ParquetConverter {
public:
    ParquetConverter() = default;

    OwnedBuffer convert_to_parquet(const ColumnarBatch& batch);
    Result<ColumnarBatch> read_parquet(Span<const byte_t> parquet_bytes);
};

}
