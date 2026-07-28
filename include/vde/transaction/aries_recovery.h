#pragma once

#include "vde/common/types.h"
#include "vde/transaction/transaction_manager.h"
#include "vde/transaction/write_ahead_log.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace vde {

struct DirtyPageEntry {
    uint32_t page_id;
    uint64_t rec_lsn;
};

struct ActiveTxEntry {
    tx_id_t tx_id;
    TransactionState state;
    uint64_t last_lsn;
};

class AriesRecoveryEngine {
public:
    explicit AriesRecoveryEngine(WriteAheadLog* wal);
    ~AriesRecoveryEngine() = default;

    Status run_recovery_pass();

    void analysis_pass();
    void redo_pass();
    void undo_pass();

    size_t dirty_page_count() const { return dirty_page_table_.size(); }
    size_t active_tx_count() const { return active_tx_table_.size(); }

    uint64_t smallest_rec_lsn() const;

private:
    WriteAheadLog* wal_;
    std::unordered_map<uint32_t, DirtyPageEntry> dirty_page_table_;
    std::unordered_map<tx_id_t, ActiveTxEntry> active_tx_table_;
    uint64_t checkpoint_lsn_ = 0;
};

}
