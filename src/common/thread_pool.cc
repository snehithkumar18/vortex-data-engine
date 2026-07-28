#include "vde/common/thread_pool.h"

namespace vde {

ThreadPool::ThreadPool(size_t threads) {
    if (threads == 0) threads = 1;
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    this->cv_.wait(lock, [this] {
                        return this->stop_ || !this->tasks_.empty();
                    });
                    if (this->stop_ && this->tasks_.empty()) return;
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (stop_) return;
        stop_ = true;
    }
    cv_.notify_all();
    for (std::thread &worker : workers_) {
        if (worker.joinable()) worker.join();
    }
    workers_.clear();
}

}
