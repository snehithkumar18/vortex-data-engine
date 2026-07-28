#include "vde/transaction/lock_manager.h"
#include <algorithm>

namespace vde {

LockManager& LockManager::instance() {
    static LockManager inst;
    return inst;
}

Status LockManager::acquire_lock(tx_id_t tx_id, uint64_t resource_id, LockMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    LockHead& head = lock_table_[resource_id];

    bool conflict = false;
    for (const auto& req : head.requests) {
        if (req.granted) {
            if (mode == LockMode::Exclusive || req.mode == LockMode::Exclusive) {
                if (req.tx_id != tx_id) {
                    conflict = true;
                    break;
                }
            }
        }
    }

    if (conflict) {
        head.requests.push_back({tx_id, mode, false});
        return Status::Error; // Lock conflict
    }

    head.requests.push_back({tx_id, mode, true});
    return Status::Ok;
}

Status LockManager::release_lock(tx_id_t tx_id, uint64_t resource_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = lock_table_.find(resource_id);
    if (it == lock_table_.end()) return Status::NotFound;

    auto& reqs = it->second.requests;
    reqs.erase(
        std::remove_if(reqs.begin(), reqs.end(),
            [tx_id](const LockRequest& r) { return r.tx_id == tx_id; }),
        reqs.end()
    );

    if (reqs.empty()) {
        lock_table_.erase(it);
    }
    return Status::Ok;
}

void LockManager::release_all_locks(tx_id_t tx_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = lock_table_.begin(); it != lock_table_.end(); ) {
        auto& reqs = it->second.requests;
        reqs.erase(
            std::remove_if(reqs.begin(), reqs.end(),
                [tx_id](const LockRequest& r) { return r.tx_id == tx_id; }),
            reqs.end()
        );
        if (reqs.empty()) {
            it = lock_table_.erase(it);
        } else {
            ++it;
        }
    }
}

bool LockManager::has_deadlock() const {
    return false; // Simplified cycle detector
}

} // namespace vde
