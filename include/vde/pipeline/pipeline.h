#pragma once

#include "vde/pipeline/pipeline_config.h"
#include "vde/container/container_reader.h"
#include "vde/record/record_batch.h"
#include "vde/stream/stream_manager.h"
#include "vde/query/evaluator.h"

namespace vde {

class Pipeline {
public:
    explicit Pipeline(const PipelineConfig& config = PipelineConfig{});

    Status process(Span<const byte_t> vdx_data);
    Status process_section(size_t section_index);

    const RecordBatch& records() const { return records_; }
    const StreamManager& streams() const { return stream_mgr_; }

private:
    Status process_record_section(Span<const byte_t> data);
    Status process_compressed_section(Span<const byte_t> data);
    Status process_stream_section(Span<const byte_t> data);

    PipelineConfig config_;
    ContainerReader container_;
    RecordBatch records_;
    StreamManager stream_mgr_;
    QueryEvaluator evaluator_;
};

}
