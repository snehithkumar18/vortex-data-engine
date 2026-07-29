#pragma once

#include "vde/common/types.h"
#include <string>
#include <vector>
#include <memory>

namespace vde {

enum class TransactionState {
    Active,
    Committed,
    Aborted
};

using tx_id_t = uint64_t;

class Transaction {
public:
    explicit Transaction(tx_id_t tx_id);

    tx_id_t tx_id() const { return tx_id_; }
    TransactionState state() const { return state_; }

    void commit();
    void abort();

    void add_write_set(uint32_t page_id, uint16_t slot_id);
    const std::vector<std::pair<uint32_t, uint16_t>>& write_set() const { return write_set_; }

private:
    tx_id_t tx_id_;
    TransactionState state_ = TransactionState::Active;
    std::vector<std::pair<uint32_t, uint16_t>> write_set_;
};

class TransactionManager {
public:
    static TransactionManager& instance();

    Transaction* begin_transaction();
    void commit(Transaction* tx);
    void abort(Transaction* tx);

    tx_id_t active_tx_count() const { return transactions_.size(); }

private:
    TransactionManager() = default;
    tx_id_t next_tx_id_ = 1;
    std::vector<std::unique_ptr<Transaction>> transactions_;
};

}
