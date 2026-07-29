#include "vde/common/config_loader.h"
#include <sstream>

namespace vde {

Status ConfigLoader::parse_string(std::string_view text) {
    entries_.clear();
    std::string s_text(text);
    std::istringstream stream(s_text);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        size_t eq_pos = line.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = line.substr(0, eq_pos);
            std::string val = line.substr(eq_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);

            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);

            entries_[key] = val;
        }
    }
    return Status::Ok;
}

std::string ConfigLoader::get_string(const std::string& key, const std::string& fallback) const {
    auto it = entries_.find(key);
    if (it != entries_.end()) return it->second;
    return fallback;
}

int64_t ConfigLoader::get_int(const std::string& key, int64_t fallback) const {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
        try {
            return std::stoll(it->second);
        } catch (...) {}
    }
    return fallback;
}

bool ConfigLoader::get_bool(const std::string& key, bool fallback) const {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
        if (it->second == "true" || it->second == "1" || it->second == "yes") return true;
        if (it->second == "false" || it->second == "0" || it->second == "no") return false;
    }
    return fallback;
}

}
