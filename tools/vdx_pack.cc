#include "vde/common/byte_writer.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: vdx_pack <out_file.vdx>" << std::endl;
        return 1;
    }

    vde::ByteWriter writer;
    writer.write_bytes(vde::Span<const vde::byte_t>(reinterpret_cast<const vde::byte_t*>(vde::kMagicBytes), 4));
    writer.write_u8(1);
    writer.write_u8(2);
    writer.write_u16_le(0);
    writer.write_u32_le(1);
    writer.write_u64_le(60);
    writer.write_u32_le(0);
    writer.write_u64_le(0);


    writer.write_u16_le(0);
    writer.write_u16_le(0);
    writer.write_u64_le(52);
    writer.write_u64_le(8);


    writer.write_u32_le(100);
    writer.write_u32_le(0);

    vde::OwnedBuffer buf = writer.release();
    std::ofstream out(argv[1], std::ios::binary);
    out.write(reinterpret_cast<const char*>(buf.data()), buf.size());

    std::cout << "Packed valid VDX file: " << argv[1] << " (" << buf.size() << " bytes)" << std::endl;
    return 0;
}
