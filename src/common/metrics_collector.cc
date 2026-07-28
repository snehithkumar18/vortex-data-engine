#include "vde/common/metrics_collector.h"

namespace vde {

MetricsCollector& MetricsCollector::instance() {
    static MetricsCollector inst;
    return inst;
}

MetricsSnapshot MetricsCollector::snapshot() const {
    return {
        bytes_read_,
        bytes_written_,
        records_processed_,
        queries_executed_,
        cache_hits_,
        cache_misses_
    };
}

std::string MetricsCollector::print_summary() const {
    std::string summary = "=== VDE ENGINE METRICS ===\n";
    summary += "Bytes Read: " + std::to_string(bytes_read_) + "\n";
    summary += "Bytes Written: " + std::to_string(bytes_written_) + "\n";
    summary += "Records Processed: " + std::to_string(records_processed_) + "\n";
    summary += "Queries Executed: " + std::to_string(queries_executed_) + "\n";
    summary += "Cache Hits: " + std::to_string(cache_hits_) + "\n";
    summary += "Cache Misses: " + std::to_string(cache_misses_) + "\n";
    return summary;
}

void MetricsCollector::reset() {
    bytes_read_ = 0;
    bytes_written_ = 0;
    records_processed_ = 0;
    queries_executed_ = 0;
    cache_hits_ = 0;
    cache_misses_ = 0;
}

} // namespace vde
