#include "vde/storage/slotted_page.h"

namespace vde {

SlottedPage::SlottedPage() {
    header_ = reinterpret_cast<PageHeader*>(data_);
    slots_ = reinterpret_cast<Slot*>(data_ + sizeof(PageHeader));
    init(0);
}

SlottedPage::SlottedPage(uint32_t page_id) {
    header_ = reinterpret_cast<PageHeader*>(data_);
    slots_ = reinterpret_cast<Slot*>(data_ + sizeof(PageHeader));
    init(page_id);
}

Status SlottedPage::init(uint32_t page_id) {
    std::memset(data_, 0, kPageSize);
    header_->magic = kSlottedPageMagic;
    header_->page_id = page_id;
    header_->slot_count = 0;
    header_->free_space_pointer = static_cast<uint16_t>(kPageSize);
    header_->lsn = 0;
    return Status::Ok;
}

uint16_t SlottedPage::free_space() const {
    uint32_t slots_end = sizeof(PageHeader) + header_->slot_count * sizeof(Slot);
    if (header_->free_space_pointer < slots_end) return 0;
    return header_->free_space_pointer - static_cast<uint16_t>(slots_end);
}

int SlottedPage::insert_tuple(Span<const byte_t> tuple_data) {
    uint16_t needed = static_cast<uint16_t>(sizeof(Slot) + tuple_data.size());
    if (free_space() < needed) return -1; // Page full

    uint16_t new_offset = header_->free_space_pointer - static_cast<uint16_t>(tuple_data.size());
    header_->free_space_pointer = new_offset;

    std::memcpy(data_ + new_offset, tuple_data.data(), tuple_data.size());

    uint16_t slot_id = header_->slot_count++;
    slots_[slot_id].offset = new_offset;
    slots_[slot_id].length = static_cast<uint16_t>(tuple_data.size());

    return slot_id;
}

Status SlottedPage::get_tuple(uint16_t slot_id, OwnedBuffer* out_tuple) const {
    if (slot_id >= header_->slot_count || !out_tuple) return Status::InvalidArgument;
    const Slot& slot = slots_[slot_id];
    if (slot.length == 0) return Status::NotFound; // Deleted tuple

    out_tuple->clear();
    out_tuple->append(data_ + slot.offset, slot.length);
    return Status::Ok;
}

Status SlottedPage::delete_tuple(uint16_t slot_id) {
    if (slot_id >= header_->slot_count) return Status::InvalidArgument;
    slots_[slot_id].length = 0;
    return Status::Ok;
}

} // namespace vde
