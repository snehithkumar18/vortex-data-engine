#include "vde/common/buffer_pool.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace vde {

BufferPool::BufferPool(size_t block_size, size_t max_blocks)
    : block_size_(block_size), max_blocks_(max_blocks) {}

BufferPool::~BufferPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (byte_t* ptr : free_list_) {
        std::free(ptr);
    }
    free_list_.clear();
}

byte_t* BufferPool::acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!free_list_.empty()) {
        byte_t* ptr = free_list_.back();
        free_list_.pop_back();
        return ptr;
    }
    return static_cast<byte_t*>(std::malloc(block_size_));
}

void BufferPool::release(byte_t* ptr) {
    if (!ptr) return;
    std::lock_guard<std::mutex> lock(mutex_);
    if (free_list_.size() < max_blocks_) {
        free_list_.push_back(ptr);
    } else {
        std::free(ptr);
    }
}

size_t BufferPool::available_blocks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return free_list_.size();
}

RingBuffer::RingBuffer(size_t capacity)
    : buffer_(capacity), capacity_(capacity) {}

RingBuffer::~RingBuffer() = default;

size_t RingBuffer::available_read() const {
    return count_;
}

size_t RingBuffer::available_write() const {
    return capacity_ - count_;
}

void RingBuffer::clear() {
    head_ = 0;
    tail_ = 0;
    count_ = 0;
}

size_t RingBuffer::write(Span<const byte_t> data) {
    size_t to_write = std::min(data.size(), available_write());
    if (to_write == 0) return 0;

    size_t first_part = std::min(to_write, capacity_ - tail_);
    std::memcpy(buffer_.data() + tail_, data.data(), first_part);

    if (to_write > first_part) {
        size_t second_part = to_write - first_part;
        std::memcpy(buffer_.data(), data.data() + first_part, second_part);
        tail_ = second_part;
    } else {
        tail_ = (tail_ + first_part) % capacity_;
    }

    count_ += to_write;
    return to_write;
}

size_t RingBuffer::read(byte_t* dest, size_t len) {
    size_t to_read = std::min(len, available_read());
    if (to_read == 0) return 0;

    size_t first_part = std::min(to_read, capacity_ - head_);
    std::memcpy(dest, buffer_.data() + head_, first_part);

    if (to_read > first_part) {
        size_t second_part = to_read - first_part;
        std::memcpy(dest + first_part, buffer_.data(), second_part);
        head_ = second_part;
    } else {
        head_ = (head_ + first_part) % capacity_;
    }

    count_ -= to_read;
    return to_read;
}

}
