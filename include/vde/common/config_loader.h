#pragma once

#include "vde/common/types.h"
#include <string>
#include <unordered_map>

namespace vde {

class ConfigLoader {
public:
    ConfigLoader() = default;

    Status parse_string(std::string_view text);
    std::string get_string(const std::string& key, const std::string& fallback = "") const;
    int64_t get_int(const std::string& key, int64_t fallback = 0) const;
    bool get_bool(const std::string& key, bool fallback = false) const;

    bool has_key(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> entries_;
};

} // namespace vde
