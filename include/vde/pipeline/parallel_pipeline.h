#pragma once

#include "vde/pipeline/pipeline.h"
#include "vde/common/thread_pool.h"
#include <vector>
#include <memory>

namespace vde {

class ParallelPipeline {
public:
    explicit ParallelPipeline(size_t worker_threads = 4, const PipelineConfig& config = PipelineConfig{});
    ~ParallelPipeline() = default;

    Status process_batch(const std::vector<Span<const byte_t>>& vdx_files);
    const std::vector<RecordBatch>& results() const { return batch_results_; }

private:
    PipelineConfig config_;
    ThreadPool pool_;
    std::vector<RecordBatch> batch_results_;
    std::mutex results_mutex_;
};

} // namespace vde
