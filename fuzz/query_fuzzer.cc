#include "vde/query/expression.h"
#include "vde/query/evaluator.h"
#include "vde/query/index.h"
#include "vde/query/transaction_cache.h"
#include "vde/record/record_decoder.h"
#include "vde/record/record_batch.h"
#include "vde/common/byte_reader.h"
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 8) return 0;

    vde::Span<const vde::byte_t> input(data, size);


    vde::ExprNode* root = nullptr;
    {
        vde::ByteReader br(input);
        vde::parse_expression(br, &root);
    }

    if (root) {
        vde::Record record;
        record.id = 1;
        vde::ByteReader br2(input);
        for (int i = 0; i < 5 && br2.remaining() > 4; ++i) {
            auto type_byte = br2.read_u8();
            if (type_byte.has_value()) {
                switch (type_byte.value() % 4) {
                    case 0: record.fields.emplace_back(br2.read_u32_le().value_or(0)); break;
                    case 1: record.fields.emplace_back(static_cast<int64_t>(br2.read_i64_le().value_or(0))); break;
                    case 2: record.fields.emplace_back(br2.read_f64_le().value_or(0.0)); break;
                    case 3: record.fields.emplace_back("fuzz", 4); break;
                }
            }
        }

        vde::QueryEvaluator evaluator;
        evaluator.evaluate(root, record);

        vde::RecordBatch batch;
        batch.add_record(std::move(record));
        std::vector<size_t> matches;
        evaluator.filter(root, batch, &matches);

        vde::free_expression(root);
    }


    {
        vde::RecordBatch batch;
        vde::ByteReader br3(input);
        vde::Schema schema;
        schema.parse(br3);
        vde::RecordDecoder decoder;
        for (int i = 0; i < 10 && br3.remaining() > 8; ++i) {
            vde::Record rec;
            if (decoder.decode(br3, schema, &rec) == vde::Status::Ok) {
                batch.add_record(std::move(rec));
            }
        }
        if (batch.record_count() > 0) {
            vde::SortedIndex index;
            index.build(batch, 0);
            if (index.is_built()) {
                vde::FieldValue key(uint32_t(42));
                std::vector<size_t> results;
                index.lookup(key, &results);
            }
        }
    }


    {
        vde::TransactionCache cache;
        cache.begin();
        vde::ByteReader br4(input);
        for (int i = 0; i < 5 && br4.remaining() > 4; ++i) {
            auto val = br4.read_u32_le();
            if (val.has_value()) {
                cache.store("key" + std::to_string(i), vde::FieldValue(val.value()));
            }
        }
        if (size % 2 == 0) {
            cache.commit();
            cache.begin();
            cache.store("new_key", vde::FieldValue(uint32_t(999)));
            cache.rollback();
        } else {
            cache.rollback();
        }
    }

    return 0;
}
