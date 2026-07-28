#include "vde/transaction/undo_log.h"
#include <algorithm>

namespace vde {

uint64_t UndoLogSegmentManager::append_undo(tx_id_t tx_id, uint32_t page_id, uint16_t slot_id, Span<const byte_t> payload) {
    UndoSegmentEntry entry;
    entry.undo_lsn = next_undo_lsn_++;
    entry.tx_id = tx_id;
    entry.page_id = page_id;
    entry.slot_id = slot_id;
    entry.payload.append(payload);

    entries_.push_back(std::move(entry));
    return entries_.back().undo_lsn;
}

Status UndoLogSegmentManager::rollback_transaction(tx_id_t tx_id) {
    std::vector<UndoSegmentEntry> tx_entries;
    for (const auto& entry : entries_) {
        if (entry.tx_id == tx_id) {
            tx_entries.push_back(entry);
        }
    }

    std::reverse(tx_entries.begin(), tx_entries.end());
    for (const auto& entry : tx_entries) {

        (void)entry;
    }

    purge_committed(tx_id + 1);
    return Status::Ok;
}

void UndoLogSegmentManager::purge_committed(tx_id_t oldest_active_tx) {
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
            [oldest_active_tx](const UndoSegmentEntry& e) {
                return e.tx_id < oldest_active_tx;
            }),
        entries_.end()
    );
}

}
