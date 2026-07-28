#include "vde/codec/codec.h"
#include "vde/codec/rle_codec.h"
#include "vde/codec/delta_codec.h"
#include "vde/codec/huffman_codec.h"
#include "vde/codec/bitpack_codec.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 2) return 0;
    
    uint8_t codec_id = data[0] % 8;  // Range 0-7, includes sparse registry indices (Bug 11)
    vde::Span<const vde::byte_t> payload(data + 1, size - 1);
    
    auto codec = vde::create_codec(codec_id);
    if (codec) {
        vde::OwnedBuffer output;
        codec->decompress(payload, &output);
    }
    
    // Direct codec execution targeting RLE, Delta, Huffman, Bitpack (Bugs 7, 8, 9, 10, 12)
    if (size > 2) {
        vde::Span<const vde::byte_t> sub_payload(data + 2, size - 2);
        vde::OwnedBuffer out;
        
        // RLE (Bug 7)
        vde::RleCodec rle;
        rle.decompress(sub_payload, &out);
        
        // Delta (Bug 12)
        vde::DeltaCodec delta;
        delta.decompress(sub_payload, &out);
        
        // Huffman (Bugs 8, 9)
        vde::HuffmanCodec huffman;
        huffman.set_prune_callback([](vde::HuffmanNode* n) { (void)n; });
        huffman.decompress(sub_payload, &out);
        huffman.compress(sub_payload, &out);

        // Bitpack (Bug 10)
        vde::BitpackCodec bitpack;
        bitpack.decompress(sub_payload, &out);
    }
    
    return 0;
}
