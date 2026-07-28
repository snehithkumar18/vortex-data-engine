#include "vde/codec/rle_codec.h"
#include "vde/codec/delta_codec.h"
#include "vde/codec/huffman_codec.h"
#include "vde/codec/bitpack_codec.h"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "--- VDE Codec Benchmark ---" << std::endl;
    std::vector<vde::byte_t> data(1024 * 1024, 0x42);

    vde::RleCodec rle;
    vde::OwnedBuffer out;

    auto t1 = std::chrono::high_resolution_clock::now();
    rle.compress(vde::Span<const vde::byte_t>(data.data(), data.size()), &out);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "RLE Compress 1MB: " << duration << " us, Compressed Size: " << out.size() << " bytes" << std::endl;

    return 0;
}
