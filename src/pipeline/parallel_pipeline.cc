#include "vde/pipeline/parallel_pipeline.h"

namespace vde {

ParallelPipeline::ParallelPipeline(size_t worker_threads, const PipelineConfig& config)
    : config_(config), pool_(worker_threads) {}

Status ParallelPipeline::process_batch(const std::vector<Span<const byte_t>>& vdx_files) {
    std::vector<std::future<Result<RecordBatch>>> futures;

    for (const auto& file_span : vdx_files) {
        futures.push_back(pool_.enqueue([file_span, this]() -> Result<RecordBatch> {
            Pipeline p(this->config_);
            Status st = p.process(file_span);
            if (st != Status::Ok) return Result<RecordBatch>::error(st);
            return Result<RecordBatch>::ok(p.records());
        }));
    }

    for (auto& fut : futures) {
        auto res = fut.get();
        if (res.has_value()) {
            std::lock_guard<std::mutex> lock(results_mutex_);
            batch_results_.push_back(std::move(res.value));
        }
    }

    return Status::Ok;
}

}
