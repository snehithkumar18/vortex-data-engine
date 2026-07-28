#include "vde/pipeline/pipeline.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 32) return 0;
    
    vde::PipelineConfig config;
    config.max_section_count = 64;
    config.max_record_size = 1024 * 1024;
    config.max_decompressed_size = 4 * 1024 * 1024;
    config.max_stream_sessions = 32;
    config.validate_checksums = false;
    
    vde::Pipeline pipeline(config);
    vde::Span<const vde::byte_t> input(data, size);
    pipeline.process(input);
    
    return 0;
}
