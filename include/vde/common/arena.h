#pragma once

#include "vde/common/types.h"
#include <cstdlib>
#include <cstring>
#include <mutex>

namespace vde {

struct ArenaPage {
    byte_t* data;
    size_t used;
    size_t capacity;
    ArenaPage* next;
};


class Arena {
public:
    explicit Arena(size_t page_size = 4096);
    ~Arena();

    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;


    void* allocate(size_t size, size_t alignment = 8);


    void reset();

    size_t total_allocated() const { return total_allocated_; }
    size_t page_count() const { return page_count_; }

private:
    ArenaPage* allocate_new_page(size_t min_size);
    void free_page_chain(ArenaPage* page);

    ArenaPage* first_page_;
    ArenaPage* current_page_;
    size_t default_page_size_;
    size_t total_allocated_;
    size_t page_count_;


    std::mutex alloc_mutex_;
    size_t peak_usage_;
};

}
