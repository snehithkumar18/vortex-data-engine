#include "vde/common/byte_reader.h"
#include <cstring>

namespace vde {

ByteReader::ByteReader() : buffer_(Span<const byte_t>()), pos_(0) {}
ByteReader::ByteReader(Span<const byte_t> buffer) : buffer_(buffer), pos_(0) {}
ByteReader::ByteReader(const byte_t* data, size_t size) : buffer_(Span<const byte_t>(data, size)), pos_(0) {}

bool ByteReader::has_remaining(size_t n) const {
    return pos_ + n <= buffer_.size();
}

size_t ByteReader::remaining() const {
    return buffer_.size() - pos_;
}

size_t ByteReader::position() const {
    return pos_;
}

size_t ByteReader::total() const {
    return buffer_.size();
}

Status ByteReader::skip(size_t n) {
    if (!has_remaining(n)) {
        return Status::Eof;
    }
    pos_ += n;
    return Status::Ok;
}

Result<uint8_t> ByteReader::peek_u8() const {
    if (!has_remaining(1)) {
        return Result<uint8_t>(Status::Eof, "End of buffer");
    }
    return Result<uint8_t>(buffer_[pos_]);
}

Result<uint8_t> ByteReader::read_u8() {
    if (!has_remaining(1)) {
        return Result<uint8_t>(Status::Eof, "End of buffer");
    }
    return Result<uint8_t>(buffer_[pos_++]);
}

Result<uint16_t> ByteReader::read_u16_le() {
    if (!has_remaining(2)) return Result<uint16_t>(Status::Eof);
    uint16_t val = static_cast<uint16_t>(buffer_[pos_]) |
                   (static_cast<uint16_t>(buffer_[pos_ + 1]) << 8);
    pos_ += 2;
    return Result<uint16_t>(val);
}

Result<uint32_t> ByteReader::read_u32_le() {
    if (!has_remaining(4)) return Result<uint32_t>(Status::Eof);
    uint32_t val = static_cast<uint32_t>(buffer_[pos_]) |
                   (static_cast<uint32_t>(buffer_[pos_ + 1]) << 8) |
                   (static_cast<uint32_t>(buffer_[pos_ + 2]) << 16) |
                   (static_cast<uint32_t>(buffer_[pos_ + 3]) << 24);
    pos_ += 4;
    return Result<uint32_t>(val);
}

Result<uint64_t> ByteReader::read_u64_le() {
    if (!has_remaining(8)) return Result<uint64_t>(Status::Eof);
    uint64_t val = 0;
    for (int i = 0; i < 8; ++i) {
        val |= (static_cast<uint64_t>(buffer_[pos_ + i]) << (i * 8));
    }
    pos_ += 8;
    return Result<uint64_t>(val);
}

Result<int8_t> ByteReader::read_i8() {
    auto res = read_u8();
    if (!res.has_value()) return Result<int8_t>(res.status, res.error_message);
    return Result<int8_t>(static_cast<int8_t>(res.get()));
}

Result<int16_t> ByteReader::read_i16_le() {
    auto res = read_u16_le();
    if (!res.has_value()) return Result<int16_t>(res.status, res.error_message);
    return Result<int16_t>(static_cast<int16_t>(res.get()));
}

Result<int32_t> ByteReader::read_i32_le() {
    auto res = read_u32_le();
    if (!res.has_value()) return Result<int32_t>(res.status, res.error_message);
    return Result<int32_t>(static_cast<int32_t>(res.get()));
}

Result<int64_t> ByteReader::read_i64_le() {
    auto res = read_u64_le();
    if (!res.has_value()) return Result<int64_t>(res.status, res.error_message);
    return Result<int64_t>(static_cast<int64_t>(res.get()));
}

Result<float> ByteReader::read_f32_le() {
    auto res = read_u32_le();
    if (!res.has_value()) return Result<float>(res.status, res.error_message);
    float f;
    std::memcpy(&f, &res.get(), sizeof(f));
    return Result<float>(f);
}

Result<double> ByteReader::read_f64_le() {
    auto res = read_u64_le();
    if (!res.has_value()) return Result<double>(res.status, res.error_message);
    double d;
    std::memcpy(&d, &res.get(), sizeof(d));
    return Result<double>(d);
}

Result<Span<const byte_t>> ByteReader::read_bytes(size_t n) {
    if (!has_remaining(n)) return Result<Span<const byte_t>>(Status::Eof);
    Span<const byte_t> sp = buffer_.subspan(pos_, n);
    pos_ += n;
    return Result<Span<const byte_t>>(sp);
}

Result<uint64_t> ByteReader::read_vlq() {
    uint64_t result = 0;
    int shift = 0;
    while (true) {
        if (!has_remaining(1)) return Result<uint64_t>(Status::Eof);
        uint8_t b = buffer_[pos_++];
        result |= (static_cast<uint64_t>(b & 0x7F) << shift);
        if ((b & 0x80) == 0) break;
        shift += 7;
        if (shift >= 64) {
            return Result<uint64_t>(Status::Corrupt, "VLQ too large");
        }
    }
    return Result<uint64_t>(result);
}

}
