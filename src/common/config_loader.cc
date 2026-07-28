#include "vde/common/config_loader.h"
#include <sstream>

namespace vde {

Status ConfigLoader::parse_string(std::string_view text) {
    entries_.clear();
    std::istringstream stream(std::string(text));
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
        std::string val = it->second;
        if (val == "true" || val == "1" || val == "yes") return true;
        if (val == "false" || val == "0" || val == "no") return false;
    }
    return fallback;
}

bool ConfigLoader::has_key(const std::string& key) const {
    return entries_.find(key) != entries_.end();
}

}
