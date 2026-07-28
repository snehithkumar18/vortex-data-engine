#include "vde/catalog/system_catalogs.h"

namespace vde {

RecordBatch SystemCatalogs::build_sys_tables_batch() {
    RecordBatch batch;
    Catalog& cat = Catalog::instance();
    auto table_names = cat.list_tables();

    uint32_t id = 1;
    for (const auto& name : table_names) {
        Record r;
        r.id = id++;
        r.fields.emplace_back(name.c_str(), name.size());
        batch.add_record(std::move(r));
    }

    return batch;
}

RecordBatch SystemCatalogs::build_sys_columns_batch() {
    RecordBatch batch;
    Catalog& cat = Catalog::instance();
    auto table_names = cat.list_tables();

    uint32_t id = 1;
    for (const auto& tname : table_names) {
        const TableDescriptor* desc = cat.get_table(tname);
        if (desc) {
            for (const auto& col : desc->columns()) {
                Record r;
                r.id = id++;
                r.fields.emplace_back(tname.c_str(), tname.size());
                r.fields.emplace_back(col.name.c_str(), col.name.size());
                r.fields.emplace_back(static_cast<uint32_t>(col.type));
                batch.add_record(std::move(r));
            }
        }
    }

    return batch;
}

RecordBatch SystemCatalogs::build_sys_indexes_batch() {
    RecordBatch batch;
    return batch;
}

}
