#pragma once

#include "vde/common/types.h"
#include "vde/transaction/transaction_manager.h"
#include <vector>
#include <cstdint>

namespace vde {

struct UndoSegmentEntry {
    uint64_t undo_lsn;
    tx_id_t tx_id;
    uint32_t page_id;
    uint16_t slot_id;
    OwnedBuffer payload;
};

class UndoLogSegmentManager {
public:
    UndoLogSegmentManager() = default;

    uint64_t append_undo(tx_id_t tx_id, uint32_t page_id, uint16_t slot_id, Span<const byte_t> payload);
    Status rollback_transaction(tx_id_t tx_id);

    size_t segment_count() const { return entries_.size(); }
    void purge_committed(tx_id_t oldest_active_tx);

private:
    std::vector<UndoSegmentEntry> entries_;
    uint64_t next_undo_lsn_ = 1;
};

}
