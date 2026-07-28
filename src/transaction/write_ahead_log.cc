#include "vde/transaction/write_ahead_log.h"

namespace vde {

uint64_t WriteAheadLog::append_record(LogRecord record) {
    record.lsn = current_lsn_++;
    log_records_.push_back(std::move(record));
    return record.lsn;
}

Status WriteAheadLog::flush() {
    return Status::Ok;
}

std::vector<LogRecord> WriteAheadLog::read_all_records() const {
    return log_records_;
}

} // namespace vde
