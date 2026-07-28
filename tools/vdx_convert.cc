#include "vde/pipeline/pipeline.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: vdx_convert <file.vdx>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) return 1;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<vde::byte_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    vde::Pipeline pipeline;
    vde::Status st = pipeline.process(vde::Span<const vde::byte_t>(buffer.data(), buffer.size()));
    if (st != vde::Status::Ok) {
        std::cerr << "Conversion error: " << vde::status_to_string(st) << std::endl;
        return 1;
    }

    const auto& batch = pipeline.records();
    std::cout << "{\n  \"record_count\": " << batch.record_count() << ",\n  \"records\": [\n";
    for (size_t i = 0; i < batch.record_count(); ++i) {
        const auto& rec = batch.record_at(i);
        std::cout << "    { \"id\": " << rec.id << " }" << (i + 1 < batch.record_count() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";

    return 0;
}
