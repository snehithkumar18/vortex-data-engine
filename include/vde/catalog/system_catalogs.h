#pragma once

#include "vde/telemetry_registry/telemetry_registry.h"
#include "vde/record/record_batch.h"
#include <string>

namespace vde {

class SystemTelemetryRegistrys {
public:
    static RecordBatch build_sys_tables_batch();
    static RecordBatch build_sys_columns_batch();
    static RecordBatch build_sys_indexes_batch();
};

}
