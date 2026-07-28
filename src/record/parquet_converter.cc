#include "vde/record/parquet_converter.h"
#include "vde/common/byte_writer.h"

namespace vde {

OwnedBuffer ParquetConverter::convert_to_parquet(const ColumnarBatch& batch) {
    ByteWriter writer;
    writer.write_bytes(Span<const byte_t>(reinterpret_cast<const byte_t*>("PAR1"), 4));
    writer.write_u32_le(static_cast<uint32_t>(batch.column_count()));
    writer.write_u32_le(static_cast<uint32_t>(batch.row_count()));
    writer.write_bytes(Span<const byte_t>(reinterpret_cast<const byte_t*>("PAR1"), 4));
    return writer.release();
}

Result<ColumnarBatch> ParquetConverter::read_parquet(Span<const byte_t> parquet_bytes) {
    (void)parquet_bytes;
    ColumnarBatch batch;
    return Result<ColumnarBatch>::ok(std::move(batch));
}

}
