#pragma once

#include "vde/codec/codec.h"

namespace vde {

class VByteCodec : public ICodec {
public:
    VByteCodec() = default;
    ~VByteCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "VByte"; }
    uint16_t id() const override { return 8; }
};

} // namespace vde
