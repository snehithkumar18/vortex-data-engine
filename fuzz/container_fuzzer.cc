#include "vde/container/container_reader.h"
#include "vde/container/file_header.h"
#include "vde/container/section_table.h"
#include "vde/container/metadata_block.h"
#include "vde/common/byte_reader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) return 0;
    
    vde::Span<const vde::byte_t> input(data, size);
    
    // Level 1: Test full container parsing
    {
        vde::ContainerReader reader;
        auto status = reader.open(input);
        if (status == vde::Status::Ok) {
            for (size_t i = 0; i < reader.sections().count(); ++i) {
                auto result = reader.read_section(i);
                if (result.has_value()) {
                    reader.dispatch_section(i, nullptr);
                }
            }
            reader.read_metadata();
        }
    }
    
    // Level 2: Direct section table parsing (bypasses file header validation)
    {
        vde::ByteReader br(input);
        vde::SectionTable table;
        uint32_t count = (size > 4) ? *reinterpret_cast<const uint32_t*>(data) % 100 : 1;
        table.parse(br, count);
        if (table.count() > 0) {
            table.find_by_offset(0x1000);
            table.section_data(0, input);
        }
    }
    
    // Level 3: Direct metadata tree parsing
    {
        vde::ByteReader br(input);
        vde::MetadataNode node;
        node.parse(br);
    }
    
    return 0;
}
