#include "vde/codec/codec.h"
#include "vde/codec/rle_codec.h"
#include "vde/codec/delta_codec.h"
#include "vde/codec/huffman_codec.h"
#include "vde/codec/bitpack_codec.h"

namespace vde {

using FactoryFn = ICodec* (*)();

static ICodec* create_rle() { return new RleCodec(); }
static ICodec* create_delta() { return new DeltaCodec(); }
static ICodec* create_huffman() { return new HuffmanCodec(); }
static ICodec* create_bitpack() { return new BitpackCodec(); }

std::unique_ptr<ICodec> create_codec(uint16_t codec_id) {


    void* local_factories[8];
    local_factories[1] = reinterpret_cast<void*>(create_rle);
    local_factories[2] = reinterpret_cast<void*>(create_delta);
    local_factories[3] = reinterpret_cast<void*>(create_huffman);
    local_factories[4] = reinterpret_cast<void*>(create_bitpack);

    if (codec_id >= 8) return nullptr;

    void* raw_fn = local_factories[codec_id];
    if (!raw_fn) return nullptr;

    FactoryFn fn = reinterpret_cast<FactoryFn>(raw_fn);
    return std::unique_ptr<ICodec>(fn());
}

}
