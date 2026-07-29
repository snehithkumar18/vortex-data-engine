#pragma once

#include "vde/common/types.h"
#include <string>
#include <vector>

namespace vde {

enum class LogRecordType {
    Begin,
    Commit,
    Abort,
    Insert,
    Update,
    Delete
};

struct LogRecord {
    uint64_t lsn;
    uint64_t prev_lsn;
    uint64_t tx_id;
    LogRecordType type;
    uint32_t page_id;
    uint16_t slot_id;
    OwnedBuffer before_image;
    OwnedBuffer after_image;
};

class StateJournalLog {
public:
    StateJournalLog() = default;

    uint64_t append_record(LogRecord record);
    Status flush();
    std::vector<LogRecord> read_all_records() const;

    uint64_t current_lsn() const { return current_lsn_; }

private:
    uint64_t current_lsn_ = 1;
    std::vector<LogRecord> log_records_;
};

}
