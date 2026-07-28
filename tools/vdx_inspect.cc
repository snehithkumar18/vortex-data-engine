#include "vde/container/container_reader.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: vdx_inspect <file.vdx>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<vde::byte_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read file data" << std::endl;
        return 1;
    }

    vde::ContainerReader reader;
    vde::Status st = reader.open(vde::Span<const vde::byte_t>(buffer.data(), buffer.size()));
    if (st != vde::Status::Ok) {
        std::cerr << "Failed to parse container: " << vde::status_to_string(st) << std::endl;
        return 1;
    }

    const auto& hdr = reader.header();
    std::cout << "--- VDX Container Header ---" << std::endl;
    std::cout << "Version: " << static_cast<int>(hdr.version_major) << "." << static_cast<int>(hdr.version_minor) << std::endl;
    std::cout << "Section Count: " << hdr.section_count << std::endl;
    std::cout << "Total Size: " << hdr.total_size << " bytes" << std::endl;

    std::cout << "\n--- Section Table ---" << std::endl;
    for (size_t i = 0; i < reader.sections().count(); ++i) {
        const auto& sec = reader.sections().at(i);
        std::cout << "Section " << i << ": Type=" << static_cast<int>(sec.type)
                  << ", Offset=" << sec.offset << ", Size=" << sec.size << std::endl;
    }

    return 0;
}
