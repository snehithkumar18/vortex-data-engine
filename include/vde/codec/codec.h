#pragma once

#include "vde/common/types.h"
#include <memory>

namespace vde {

enum class CodecId : uint16_t {
    None    = 0,
    Rle     = 1,
    Delta   = 2,
    Huffman = 3,
    Bitpack = 4
};

class ICodec {
public:
    virtual ~ICodec() = default;
    virtual Status decompress(Span<const byte_t> input, OwnedBuffer* output) = 0;
    virtual Status compress(Span<const byte_t> input, OwnedBuffer* output) = 0;
    virtual const char* name() const = 0;
    virtual uint16_t id() const = 0;
};

// Factory function to instantiate codec by ID
std::unique_ptr<ICodec> create_codec(uint16_t codec_id);

} // namespace vde
