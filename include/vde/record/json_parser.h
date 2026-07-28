#pragma once

#include "vde/record/record_batch.h"
#include <string>

namespace vde {

class JsonRecordParser {
public:
    JsonRecordParser() = default;

    Result<Record> parse_record(std::string_view json_str);
    Result<RecordBatch> parse_batch(std::string_view json_str);
};

}
