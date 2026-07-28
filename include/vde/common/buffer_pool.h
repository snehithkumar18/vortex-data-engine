#pragma once

#include "vde/common/types.h"
#include <vector>
#include <memory>
#include <mutex>

namespace vde {

class BufferPool {
public:
    explicit BufferPool(size_t block_size = 4096, size_t max_blocks = 256);
    ~BufferPool();

    byte_t* acquire();
    void release(byte_t* ptr);

    size_t block_size() const { return block_size_; }
    size_t available_blocks() const;

private:
    size_t block_size_;
    size_t max_blocks_;
    std::vector<byte_t*> free_list_;
    mutable std::mutex mutex_;
};

class RingBuffer {
public:
    explicit RingBuffer(size_t capacity);
    ~RingBuffer();

    size_t write(Span<const byte_t> data);
    size_t read(byte_t* dest, size_t len);

    size_t available_read() const;
    size_t available_write() const;
    void clear();

private:
    std::vector<byte_t> buffer_;
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t count_ = 0;
    size_t capacity_;
};

}
