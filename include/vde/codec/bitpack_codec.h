#pragma once

#include "vde/codec/codec.h"

namespace vde {

class BitpackCodec : public ICodec {
public:
    BitpackCodec() = default;
    ~BitpackCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "Bitpack"; }
    uint16_t id() const override { return static_cast<uint16_t>(CodecId::Bitpack); }
};

}
