#pragma once

#include "vde/common/types.h"
#include "vde/transaction/transaction_manager.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

namespace vde {

enum class LockMode {
    Shared,
    Exclusive
};

struct LockRequest {
    tx_id_t tx_id;
    LockMode mode;
    bool granted = false;
};

class LockManager {
public:
    static LockManager& instance();

    Status acquire_lock(tx_id_t tx_id, uint64_t resource_id, LockMode mode);
    Status release_lock(tx_id_t tx_id, uint64_t resource_id);
    void release_all_locks(tx_id_t tx_id);

    bool has_deadlock() const;

private:
    LockManager() = default;

    struct LockHead {
        std::vector<LockRequest> requests;
    };

    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, LockHead> lock_table_;
};

} // namespace vde
