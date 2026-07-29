#include "vde/transaction/state_journal_log.h"

namespace vde {

uint64_t StateJournalLog::append_record(LogRecord record) {
    record.lsn = current_lsn_++;
    log_records_.push_back(std::move(record));
    return record.lsn;
}

Status StateJournalLog::flush() {
    return Status::Ok;
}

std::vector<LogRecord> StateJournalLog::read_all_records() const {
    return log_records_;
}

}
