#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace vde {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& instance();

    void set_level(LogLevel level) { min_level_ = level; }
    LogLevel level() const { return min_level_; }

    void log(LogLevel level, const char* file, int line, const std::string& message);
    const std::vector<std::string>& logs() const { return log_history_; }
    void clear();

private:
    Logger() = default;
    LogLevel min_level_ = LogLevel::Info;
    std::vector<std::string> log_history_;
    mutable std::mutex mutex_;
};

#define VDE_LOG_INFO(msg) vde::Logger::instance().log(vde::LogLevel::Info, __FILE__, __LINE__, msg)
#define VDE_LOG_WARN(msg) vde::Logger::instance().log(vde::LogLevel::Warning, __FILE__, __LINE__, msg)
#define VDE_LOG_ERROR(msg) vde::Logger::instance().log(vde::LogLevel::Error, __FILE__, __LINE__, msg)

}
