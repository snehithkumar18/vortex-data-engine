#include "vde/common/byte_writer.h"
#include <cstring>

namespace vde {

void ByteWriter::write_u8(uint8_t val) {
    buffer_.push_back(val);
}

void ByteWriter::write_u16_le(uint16_t val) {
    buffer_.push_back(static_cast<byte_t>(val & 0xFF));
    buffer_.push_back(static_cast<byte_t>((val >> 8) & 0xFF));
}

void ByteWriter::write_u32_le(uint32_t val) {
    buffer_.push_back(static_cast<byte_t>(val & 0xFF));
    buffer_.push_back(static_cast<byte_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<byte_t>((val >> 16) & 0xFF));
    buffer_.push_back(static_cast<byte_t>((val >> 24) & 0xFF));
}

void ByteWriter::write_u64_le(uint64_t val) {
    for (int i = 0; i < 8; ++i) {
        buffer_.push_back(static_cast<byte_t>((val >> (i * 8)) & 0xFF));
    }
}

void ByteWriter::write_i16_le(int16_t val) {
    write_u16_le(static_cast<uint16_t>(val));
}

void ByteWriter::write_i32_le(int32_t val) {
    write_u32_le(static_cast<uint32_t>(val));
}

void ByteWriter::write_i64_le(int64_t val) {
    write_u64_le(static_cast<uint64_t>(val));
}

void ByteWriter::write_f32_le(float val) {
    uint32_t u;
    std::memcpy(&u, &val, sizeof(u));
    write_u32_le(u);
}

void ByteWriter::write_f64_le(double val) {
    uint64_t u;
    std::memcpy(&u, &val, sizeof(u));
    write_u64_le(u);
}

void ByteWriter::write_bytes(Span<const byte_t> bytes) {
    buffer_.insert(buffer_.end(), bytes.data(), bytes.data() + bytes.size());
}

void ByteWriter::write_vlq(uint64_t val) {
    do {
        uint8_t b = val & 0x7F;
        val >>= 7;
        if (val != 0) {
            b |= 0x80;
        }
        buffer_.push_back(b);
    } while (val != 0);
}

Span<const byte_t> ByteWriter::data() const {
    return Span<const byte_t>(buffer_.data(), buffer_.size());
}

size_t ByteWriter::size() const {
    return buffer_.size();
}

OwnedBuffer ByteWriter::release() {
    return OwnedBuffer(std::move(buffer_));
}

} // namespace vde
