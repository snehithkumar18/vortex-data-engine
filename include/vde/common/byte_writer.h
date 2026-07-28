#pragma once

#include "vde/common/types.h"
#include <vector>

namespace vde {

class ByteWriter {
public:
    ByteWriter() = default;
    explicit ByteWriter(size_t reserve_size) { buffer_.reserve(reserve_size); }

    void write_u8(uint8_t val);
    void write_u16_le(uint16_t val);
    void write_i16_le(int16_t val);
    void write_u32_le(uint32_t val);
    void write_i32_le(int32_t val);
    void write_u64_le(uint64_t val);
    void write_i64_le(int64_t val);
    void write_f32_le(float val);
    void write_f64_le(double val);

    void write_bytes(Span<const byte_t> bytes);
    void write_vlq(uint64_t val);

    Span<const byte_t> data() const;
    size_t size() const;
    bool empty() const { return buffer_.empty(); }

    OwnedBuffer release();
    void clear() { buffer_.clear(); }

private:
    std::vector<byte_t> buffer_;
};

} // namespace vde
