#pragma once

#include "vde/common/types.h"
#include <cstring>

namespace vde {

class ByteReader {
public:
    ByteReader();
    explicit ByteReader(Span<const byte_t> input);
    ByteReader(const byte_t* data, size_t size);

    Result<uint8_t> read_u8();
    Result<int8_t> read_i8();
    Result<uint16_t> read_u16_le();
    Result<int16_t> read_i16_le();
    Result<uint32_t> read_u32_le();
    Result<int32_t> read_i32_le();
    Result<uint64_t> read_u64_le();
    Result<int64_t> read_i64_le();
    Result<float> read_f32_le();
    Result<double> read_f64_le();

    Result<Span<const byte_t>> read_bytes(size_t n);
    Result<uint64_t> read_vlq();

    Result<uint8_t> peek_u8() const;
    Status skip(size_t n);

    bool has_remaining(size_t n) const;
    size_t remaining() const;
    size_t position() const;
    size_t total() const;

    void reset() { pos_ = 0; }
    void seek(size_t pos) { pos_ = (pos < buffer_.size()) ? pos : buffer_.size(); }

    Span<const byte_t> remaining_span() const {
        return buffer_.subspan(pos_);
    }

private:
    Span<const byte_t> buffer_;
    size_t pos_;
};

}
