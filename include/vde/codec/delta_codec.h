#pragma once

#include "vde/codec/codec.h"

namespace vde {

class DeltaCodec : public ICodec {
public:
    DeltaCodec() = default;
    ~DeltaCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "Delta"; }
    uint16_t id() const override { return static_cast<uint16_t>(CodecId::Delta); }
};

}
