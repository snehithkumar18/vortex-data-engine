#pragma once

#include "vde/codec/codec.h"

namespace vde {

class RleCodec : public ICodec {
public:
    RleCodec() = default;
    ~RleCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "RLE"; }
    uint16_t id() const override { return static_cast<uint16_t>(CodecId::Rle); }
};

}
