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


    {
        vde::ByteReader br2(input);
        vde::RecordDecoder decoder;
        vde::Record record;
        decoder.decode(br2, schema, &record);
    }


    {
        vde::ByteReader br3(input);
        vde::RecordDecoder decoder;
        vde::Record rec;
        if (decoder.decode(br3, schema, &rec) == vde::Status::Ok) {
            for (auto& field : rec.fields) {

                field = field;

                field.to_string_lossy();
            }
        }
    }


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
