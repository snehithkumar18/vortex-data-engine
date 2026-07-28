#pragma once

#include "vde/common/types.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace vde {

static constexpr size_t kPageSize = 4096;
static constexpr uint32_t kSlottedPageMagic = 0x56445350;

struct PageHeader {
    uint32_t magic;
    uint32_t page_id;
    uint16_t slot_count;
    uint16_t free_space_pointer;
    uint32_t lsn;
};

struct Slot {
    uint16_t offset;
    uint16_t length;
};

class SlottedPage {
public:
    SlottedPage();
    explicit SlottedPage(uint32_t page_id);

    Status init(uint32_t page_id);
    int insert_tuple(Span<const byte_t> tuple_data);
    Status get_tuple(uint16_t slot_id, OwnedBuffer* out_tuple) const;
    Status delete_tuple(uint16_t slot_id);

    uint16_t free_space() const;
    uint32_t page_id() const { return header_->page_id; }

    byte_t* raw_data() { return data_; }
    const byte_t* raw_data() const { return data_; }

private:
    byte_t data_[kPageSize];
    PageHeader* header_;
    Slot* slots_;
};

}
