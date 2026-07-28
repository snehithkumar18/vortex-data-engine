#pragma once

#include "vde/common/types.h"
#include <string>
#include <vector>

namespace vde {

struct MetricsSnapshot {
    uint64_t bytes_read;
    uint64_t bytes_written;
    uint64_t records_processed;
    uint64_t queries_executed;
    uint64_t cache_hits;
    uint64_t cache_misses;
};

class MetricsCollector {
public:
    static MetricsCollector& instance();

    void record_bytes_read(size_t bytes) { bytes_read_ += bytes; }
    void record_bytes_written(size_t bytes) { bytes_written_ += bytes; }
    void record_record_processed() { records_processed_++; }
    void record_query_executed() { queries_executed_++; }
    void record_cache_hit() { cache_hits_++; }
    void record_cache_miss() { cache_misses_++; }

    MetricsSnapshot snapshot() const;
    std::string print_summary() const;
    void reset();

private:
    MetricsCollector() = default;

    uint64_t bytes_read_ = 0;
    uint64_t bytes_written_ = 0;
    uint64_t records_processed_ = 0;
    uint64_t queries_executed_ = 0;
    uint64_t cache_hits_ = 0;
    uint64_t cache_misses_ = 0;
};

} // namespace vde
