#include "vde/catalog/catalog.h"

namespace vde {

Catalog& Catalog::instance() {
    static Catalog cat;
    return cat;
}

Status Catalog::create_table(const std::string& name, Schema schema) {
    if (tables_.find(name) != tables_.end()) {
        return Status::Error;
    }
    tables_[name] = std::make_unique<TableDescriptor>(name, std::move(schema));
    return Status::Ok;
}

Status Catalog::drop_table(const std::string& name) {
    auto it = tables_.find(name);
    if (it == tables_.end()) {
        return Status::NotFound;
    }
    tables_.erase(it);
    indexes_.erase(name);
    return Status::Ok;
}

TableDescriptor* Catalog::get_table(const std::string& name) {
    auto it = tables_.find(name);
    if (it != tables_.end()) return it->second.get();
    return nullptr;
}

const TableDescriptor* Catalog::get_table(const std::string& name) const {
    auto it = tables_.find(name);
    if (it != tables_.end()) return it->second.get();
    return nullptr;
}

bool Catalog::has_table(const std::string& name) const {
    return tables_.find(name) != tables_.end();
}

std::vector<std::string> Catalog::list_tables() const {
    std::vector<std::string> res;
    res.reserve(tables_.size());
    for (const auto& [name, desc] : tables_) {
        res.push_back(name);
    }
    return res;
}

Status Catalog::register_index(const std::string& table_name, const std::string& index_name, std::unique_ptr<SortedIndex> index) {
    if (!has_table(table_name)) return Status::NotFound;
    indexes_[table_name][index_name] = std::move(index);
    return Status::Ok;
}

const SortedIndex* Catalog::get_index(const std::string& table_name, const std::string& index_name) const {
    auto t_it = indexes_.find(table_name);
    if (t_it != indexes_.end()) {
        auto i_it = t_it->second.find(index_name);
        if (i_it != t_it->second.end()) {
            return i_it->second.get();
        }
    }
    return nullptr;
}

void Catalog::clear() {
    tables_.clear();
    indexes_.clear();
}

}
