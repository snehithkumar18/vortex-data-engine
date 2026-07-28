#include "vde/pipeline/pipeline.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: vdx_dump <file.vdx>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) return 1;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<vde::byte_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    vde::ContainerReader reader;
    vde::Status st = reader.open(vde::Span<const vde::byte_t>(buffer.data(), buffer.size()));
    if (st != vde::Status::Ok) return 1;

    std::cout << "=== VDX BINARY DUMP ===" << std::endl;
    std::cout << "File Size: " << size << " bytes" << std::endl;
    std::cout << "Sections: " << reader.sections().count() << std::endl;

    for (size_t i = 0; i < reader.sections().count(); ++i) {
        const auto& sec = reader.sections().at(i);
        std::cout << "Section [" << i << "]: Type=" << static_cast<int>(sec.type)
                  << " Offset=" << sec.offset << " Size=" << sec.size << std::endl;
    }

    return 0;
}
