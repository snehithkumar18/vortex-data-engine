#include "vde/transaction/journal_recovery.h"
#include <algorithm>

namespace vde {

JournalRecoveryEngine::JournalRecoveryEngine(StateJournalLog* wal)
    : wal_(wal) {}

uint64_t JournalRecoveryEngine::smallest_rec_lsn() const {
    if (dirty_page_table_.empty()) return 0;
    uint64_t min_lsn = UINT64_MAX;
    for (const auto& [page_id, entry] : dirty_page_table_) {
        if (entry.rec_lsn < min_lsn) min_lsn = entry.rec_lsn;
    }
    return min_lsn == UINT64_MAX ? 0 : min_lsn;
}

void JournalRecoveryEngine::analysis_pass() {
    if (!wal_) return;
    auto logs = wal_->read_all_records();

    for (const auto& log : logs) {

        ActiveTxEntry& tx = active_tx_table_[log.tx_id];
        tx.tx_id = log.tx_id;
        tx.last_lsn = log.lsn;

        if (log.type == LogRecordType::Commit) {
            tx.state = TransactionState::Committed;
        } else if (log.type == LogRecordType::Abort) {
            tx.state = TransactionState::Aborted;
        } else if (log.type == LogRecordType::Begin) {
            tx.state = TransactionState::Active;
        } else if (log.type == LogRecordType::Insert || log.type == LogRecordType::Update) {
            tx.state = TransactionState::Active;
            if (dirty_page_table_.find(log.page_id) == dirty_page_table_.end()) {
                dirty_page_table_[log.page_id] = {log.page_id, log.lsn};
            }
        }
    }
}

void JournalRecoveryEngine::redo_pass() {
    if (!wal_) return;
    uint64_t start_lsn = smallest_rec_lsn();
    auto logs = wal_->read_all_records();

    for (const auto& log : logs) {
        if (log.lsn < start_lsn) continue;


        if (log.type == LogRecordType::Insert || log.type == LogRecordType::Update) {

        }
    }
}

void JournalRecoveryEngine::undo_pass() {
    if (!wal_) return;
    std::vector<tx_id_t> active_txs;
    for (const auto& [tx_id, tx_entry] : active_tx_table_) {
        if (tx_entry.state == TransactionState::Active) {
            active_txs.push_back(tx_id);
        }
    }

    if (active_txs.empty()) return;

    auto logs = wal_->read_all_records();
    std::reverse(logs.begin(), logs.end());

    for (const auto& log : logs) {
        if (std::find(active_txs.begin(), active_txs.end(), log.tx_id) != active_txs.end()) {
            if (log.type == LogRecordType::Insert || log.type == LogRecordType::Update) {

            }
        }
    }
}

Status JournalRecoveryEngine::run_recovery_pass() {
    analysis_pass();
    redo_pass();
    undo_pass();
    return Status::Ok;
}

}
