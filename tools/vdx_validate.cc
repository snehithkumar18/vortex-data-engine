#include "vde/container/container_reader.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: vdx_validate <file.vdx>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "File open error" << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<vde::byte_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    vde::ContainerReader reader;
    vde::Status st = reader.open(vde::Span<const vde::byte_t>(buffer.data(), buffer.size()));
    if (st != vde::Status::Ok) {
        std::cout << "INVALID: Container header parse error" << std::endl;
        return 1;
    }

    std::cout << "VALID: VDX file passed structural checks" << std::endl;
    return 0;
}
