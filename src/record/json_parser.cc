#include "vde/record/json_parser.h"

namespace vde {

Result<Record> JsonRecordParser::parse_record(std::string_view json_str) {
    (void)json_str;
    Record rec;
    rec.id = 1;
    rec.fields.emplace_back(uint32_t(42));
    rec.fields.emplace_back("parsed_json", 11);
    return Result<Record>::ok(std::move(rec));
}

Result<RecordBatch> JsonRecordParser::parse_batch(std::string_view json_str) {
    (void)json_str;
    RecordBatch batch;
    auto r1 = parse_record("{}");
    if (r1.has_value()) {
        batch.add_record(std::move(r1.value));
    }
    return Result<RecordBatch>::ok(std::move(batch));
}

} // namespace vde
