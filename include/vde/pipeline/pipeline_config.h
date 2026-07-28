#pragma once

#include <cstdint>
#include <cstddef>

namespace vde {

struct PipelineConfig {
    size_t max_section_count = 1024;
    size_t max_record_size = 16 * 1024 * 1024;
    size_t max_decompressed_size = 64 * 1024 * 1024;
    size_t max_stream_sessions = 256;
    uint64_t stream_timeout_ms = 30000;
    bool validate_checksums = true;
    bool enable_query_cache = true;
    uint16_t default_codec_id = 0;
};

} // namespace vde
