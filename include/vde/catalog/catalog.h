#pragma once

#include "vde/catalog/table_descriptor.h"
#include "vde/query/index.h"
#include "vde/query/btree_index.h"
#include "vde/query/hash_index.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace vde {

class Catalog {
public:
    static Catalog& instance();

    Status create_table(const std::string& name, Schema schema);
    Status drop_table(const std::string& name);

    TableDescriptor* get_table(const std::string& name);
    const TableDescriptor* get_table(const std::string& name) const;
    bool has_table(const std::string& name) const;

    std::vector<std::string> list_tables() const;

    Status register_index(const std::string& table_name, const std::string& index_name, std::unique_ptr<SortedIndex> index);
    const SortedIndex* get_index(const std::string& table_name, const std::string& index_name) const;

    void clear();

private:
    Catalog() = default;
    ~Catalog() = default;

    std::unordered_map<std::string, std::unique_ptr<TableDescriptor>> tables_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<SortedIndex>>> indexes_;
};

} // namespace vde
