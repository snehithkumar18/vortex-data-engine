#include "vde/transaction/transaction_manager.h"

namespace vde {

Transaction::Transaction(tx_id_t tx_id)
    : tx_id_(tx_id), state_(TransactionState::Active) {}

void Transaction::commit() {
    state_ = TransactionState::Committed;
}

void Transaction::abort() {
    state_ = TransactionState::Aborted;
}

void Transaction::add_write_set(uint32_t page_id, uint16_t slot_id) {
    write_set_.emplace_back(page_id, slot_id);
}

TransactionManager& TransactionManager::instance() {
    static TransactionManager inst;
    return inst;
}

Transaction* TransactionManager::begin_transaction() {
    tx_id_t tid = next_tx_id_++;
    auto tx = std::make_unique<Transaction>(tid);
    Transaction* raw = tx.get();
    transactions_.push_back(std::move(tx));
    return raw;
}

void TransactionManager::commit(Transaction* tx) {
    if (tx) {
        tx->commit();
    }
}

void TransactionManager::abort(Transaction* tx) {
    if (tx) {
        tx->abort();
    }
}

}
