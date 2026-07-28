#include "vde/common/logger.h"
#include <iostream>

namespace vde {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::log(LogLevel level, const char* file, int line, const std::string& message) {
    if (level < min_level_) return;

    std::lock_guard<std::mutex> lock(mutex_);
    const char* lvl_str = "INFO";
    switch (level) {
        case LogLevel::Debug:   lvl_str = "DEBUG"; break;
        case LogLevel::Info:    lvl_str = "INFO"; break;
        case LogLevel::Warning: lvl_str = "WARN"; break;
        case LogLevel::Error:   lvl_str = "ERROR"; break;
        case LogLevel::Fatal:   lvl_str = "FATAL"; break;
    }

    std::string formatted = "[" + std::string(lvl_str) + "] " + file + ":" + std::to_string(line) + " - " + message;
    log_history_.push_back(formatted);

#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    std::cout << formatted << std::endl;
#endif
}

void Logger::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    log_history_.clear();
}

} // namespace vde
