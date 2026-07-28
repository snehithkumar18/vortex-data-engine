#include "vde/common/byte_writer.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: vdx_pack <out_file.vdx>" << std::endl;
        return 1;
    }

    vde::ByteWriter writer;
    writer.write_bytes(vde::kMagicBytes, 4);
    writer.write_u8(1); // Major
    writer.write_u8(2); // Minor
    writer.write_u16_le(0); // Flags
    writer.write_u32_le(1); // 1 Section
    writer.write_u64_le(60); // Total size
    writer.write_u32_le(0); // CRC dummy
    writer.write_zeros(8); // Reserved

    // Section table entry (20 bytes)
    writer.write_u16_le(0); // Records type
    writer.write_u16_le(0); // Flags
    writer.write_u64_le(52); // Offset
    writer.write_u64_le(8); // Size

    // Section data (8 bytes)
    writer.write_u32_le(100); // record id
    writer.write_u32_le(0); // empty fields count

    vde::OwnedBuffer buf = writer.release();
    std::ofstream out(argv[1], std::ios::binary);
    out.write(reinterpret_cast<const char*>(buf.data()), buf.size());

    std::cout << "Packed valid VDX file: " << argv[1] << " (" << buf.size() << " bytes)" << std::endl;
    return 0;
}
