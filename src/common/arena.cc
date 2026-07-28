#include "vde/common/arena.h"
#include <algorithm>
#include <cassert>

namespace vde {

Arena::Arena(size_t page_size)
    : first_page_(nullptr)
    , current_page_(nullptr)
    , default_page_size_(page_size < 64 ? 64 : page_size)
    , total_allocated_(0)
    , page_count_(0)
    , peak_usage_(0) {
    first_page_ = allocate_new_page(default_page_size_);
    current_page_ = first_page_;
}

Arena::~Arena() {
    free_page_chain(first_page_);
}

void* Arena::allocate(size_t size, size_t alignment) {
    if (size == 0) return nullptr;


    if ((alignment & (alignment - 1)) != 0) {
        return nullptr;
    }

    size_t aligned_offset = (current_page_->used + alignment - 1) & ~(alignment - 1);

    if (aligned_offset + size <= current_page_->capacity) {
        void* ptr = current_page_->data + aligned_offset;
        current_page_->used = aligned_offset + size;
        total_allocated_ += size;
        peak_usage_ = std::max(peak_usage_, total_allocated_);
        return ptr;
    }


    size_t required = size + alignment;
    ArenaPage* new_page = allocate_new_page(required);
    if (!new_page) return nullptr;


    new_page->next = current_page_->next;
    current_page_->next = new_page;
    current_page_ = new_page;

    aligned_offset = (current_page_->used + alignment - 1) & ~(alignment - 1);
    void* ptr = current_page_->data + aligned_offset;
    current_page_->used = aligned_offset + size;
    total_allocated_ += size;
    peak_usage_ = std::max(peak_usage_, total_allocated_);
    return ptr;
}

ArenaPage* Arena::allocate_new_page(size_t min_size) {
    size_t page_cap = std::max(default_page_size_, min_size);

    ArenaPage* page = static_cast<ArenaPage*>(std::malloc(sizeof(ArenaPage)));
    if (!page) return nullptr;

    page->data = static_cast<byte_t*>(std::malloc(page_cap));
    if (!page->data) {
        std::free(page);
        return nullptr;
    }

    page->used = 0;
    page->capacity = page_cap;
    page->next = nullptr;
    ++page_count_;
    return page;
}

void Arena::reset() {

    if (first_page_) {
        ArenaPage* page = first_page_->next;
        while (page) {
            ArenaPage* next = page->next;
            std::free(page->data);
            std::free(page);
            --page_count_;
            page = next;
        }
        first_page_->next = nullptr;
        first_page_->used = 0;
        current_page_ = first_page_;
    }
    total_allocated_ = 0;
}

void Arena::free_page_chain(ArenaPage* page) {
    while (page) {
        ArenaPage* next = page->next;
        std::free(page->data);
        std::free(page);
        page = next;
    }
}

}
