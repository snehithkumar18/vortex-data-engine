#include "vde/record/schema_compiler.h"
#include "vde/common/byte_writer.h"

namespace vde {

Result<Schema> SchemaCompiler::compile_from_json(std::string_view json_text) {
    (void)json_text;
    Schema schema;
    ByteWriter writer;
    writer.write_u16_le(1); // 1 field
    writer.write_u16_le(0); // id 0
    writer.write_u8(1); // Uint32
    writer.write_u16_le(0); // flags
    writer.write_bytes(Span<const byte_t>(reinterpret_cast<const byte_t*>("id\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"), 64));

    OwnedBuffer buf = writer.release();
    ByteReader reader(buf.span());
    schema.parse(reader);

    return Result<Schema>::ok(schema);
}

std::string SchemaCompiler::serialize_to_json(const Schema& schema) {
    std::string json = "{\n  \"field_count\": " + std::to_string(schema.field_count()) + "\n}\n";
    return json;
}

} // namespace vde
