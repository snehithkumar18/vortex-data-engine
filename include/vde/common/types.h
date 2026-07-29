#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <utility>

namespace vde {

using byte_t = uint8_t;

enum class Status : uint8_t {
    Ok = 0,
    Error,
    Eof,
    Truncated,
    Corrupt,
    OutOfMemory,
    InvalidArgument,
    Unsupported,
    NotFound,
    Overflow
};

inline const char* status_to_string(Status s) {
    switch (s) {
        case Status::Ok:              return "Ok";
        case Status::Error:           return "Error";
        case Status::Eof:             return "Eof";
        case Status::Truncated:       return "Truncated";
        case Status::Corrupt:         return "Corrupt";
        case Status::OutOfMemory:     return "OutOfMemory";
        case Status::InvalidArgument: return "InvalidArgument";
        case Status::Unsupported:     return "Unsupported";
        case Status::NotFound:        return "NotFound";
        case Status::Overflow:        return "Overflow";
    }
    return "Unknown";
}

template <typename T>
struct Result {
    T value;
    Status status;
    std::string error_message;

    Result() : value{}, status(Status::Error), error_message("") {}
    Result(T val) : value(std::move(val)), status(Status::Ok), error_message("") {}
    Result(Status s, std::string msg = "") : value{}, status(s), error_message(std::move(msg)) {}
    Result(T val, Status s, std::string msg = "") : value(std::move(val)), status(s), error_message(std::move(msg)) {}

    static Result ok(T val) { return Result(std::move(val), Status::Ok); }
    static Result error(Status s, std::string msg = "") { return Result(s, std::move(msg)); }

    bool has_value() const { return status == Status::Ok; }
    bool is_ok() const { return status == Status::Ok; }

    const T& get() const { return value; }
    T& get() { return value; }

    const T& operator*() const { return value; }
    T& operator*() { return value; }
    const T* operator->() const { return &value; }
    T* operator->() { return &value; }

    T value_or(T fallback) const {
        return has_value() ? value : std::move(fallback);
    }
};

template <typename T>
struct Span {
    Span() : data_(nullptr), size_(0) {}
    Span(T* d, size_t s) : data_(d), size_(s) {}

    template <typename Container>
    explicit Span(Container& c) : data_(c.data()), size_(c.size()) {}

    T* data() const { return data_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    T& operator[](size_t i) const { return data_[i]; }

    Span<T> subspan(size_t offset) const {
        if (offset >= size_) return Span<T>(data_ + size_, 0);
        return Span<T>(data_ + offset, size_ - offset);
    }

    Span<T> subspan(size_t offset, size_t count) const {
        if (offset >= size_) return Span<T>(data_ + size_, 0);
        size_t actual = (offset + count > size_) ? (size_ - offset) : count;
        return Span<T>(data_ + offset, actual);
    }

    T* begin() const { return data_; }
    T* end() const { return data_ + size_; }

private:
    T* data_;
    size_t size_;
};

struct OwnedBuffer {
    OwnedBuffer() = default;
    explicit OwnedBuffer(size_t initial_size) : storage_(initial_size) {}
    explicit OwnedBuffer(std::vector<byte_t> data) : storage_(std::move(data)) {}

    OwnedBuffer(const OwnedBuffer&) = default;
    OwnedBuffer& operator=(const OwnedBuffer&) = default;
    OwnedBuffer(OwnedBuffer&& other) noexcept : storage_(std::move(other.storage_)) {}
    OwnedBuffer& operator=(OwnedBuffer&& other) noexcept {
        storage_ = std::move(other.storage_);
        return *this;
    }

    byte_t* data() { return storage_.data(); }
    const byte_t* data() const { return storage_.data(); }
    size_t size() const { return storage_.size(); }
    bool empty() const { return storage_.empty(); }

    void resize(size_t n) { storage_.resize(n); }
    void resize(size_t n, byte_t val) { storage_.resize(n, val); }
    void reserve(size_t n) { storage_.reserve(n); }
    void clear() { storage_.clear(); }

    void append(const byte_t* src, size_t len) {
        storage_.insert(storage_.end(), src, src + len);
    }

    void append(Span<const byte_t> span) {
        storage_.insert(storage_.end(), span.data(), span.data() + span.size());
    }

    byte_t& operator[](size_t i) { return storage_[i]; }
    const byte_t& operator[](size_t i) const { return storage_[i]; }

    Span<const byte_t> span() const {
        return Span<const byte_t>(storage_.data(), storage_.size());
    }

    Span<byte_t> mutable_span() {
        return Span<byte_t>(storage_.data(), storage_.size());
    }

    std::vector<byte_t>& vec() { return storage_; }
    const std::vector<byte_t>& vec() const { return storage_; }

private:
    std::vector<byte_t> storage_;
};

constexpr size_t kMaxSectionCount      = 1024;
constexpr size_t kMaxRecordSize        = 16 * 1024 * 1024;
constexpr size_t kMaxNestingDepth      = 64;

constexpr size_t kFileHeaderSize       = 32;
constexpr size_t kSectionEntrySize     = 20;
constexpr uint16_t kMetadataInheritKey = 0xFFFF;

constexpr char kMagicBytes[4]          = {'V', 'D', 'X', '\x01'};
constexpr uint8_t kFormatVersionMajor   = 1;
constexpr uint8_t kFormatVersionMinor   = 2;

}
