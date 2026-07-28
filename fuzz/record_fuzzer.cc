#include "vde/record/record_decoder.h"
#include "vde/record/record_batch.h"
#include "vde/record/field_value.h"
#include "vde/record/schema.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) return 0;
    
    vde::Span<const vde::byte_t> input(data, size);
    vde::ByteReader br(input);
    
    vde::Schema schema;
    schema.parse(br);
    
    // Level 1: Record decoder tests
    {
        vde::ByteReader br2(input);
        vde::RecordDecoder decoder;
        vde::Record record;
        decoder.decode(br2, schema, &record);
    }
    
    // Level 2: FieldValue copy/assignment/lossy string conversions (Bugs 19, 20)
    {
        vde::ByteReader br3(input);
        vde::RecordDecoder decoder;
        vde::Record rec;
        if (decoder.decode(br3, schema, &rec) == vde::Status::Ok) {
            for (auto& field : rec.fields) {
                // Bug 20 trigger: self-assignment in release builds
                field = field;
                // Bug 19 trigger: Uint64 fallthrough in to_string_lossy
                field.to_string_lossy();
            }
        }
    }
    
    // Level 3: RecordBatch callback compaction reentrancy (Bug 22)
    {
        vde::RecordBatch batch;
        vde::ByteReader br4(input);
        vde::RecordDecoder decoder;
        for (int i = 0; i < 5 && br4.remaining() > 10; ++i) {
            vde::Record rec;
            if (decoder.decode(br4, schema, &rec) == vde::Status::Ok) {
                batch.add_record(std::move(rec));
            }
        }
        batch.for_each([&batch](size_t idx, const vde::Record& r) {
            (void)r;
            if (idx > 0) {
                batch.compact();
            }
        });
    }
    
    return 0;
}
