#pragma once

#include "vde/codec/codec.h"

namespace vde {

class LzLiteCodec : public ICodec {
public:
    LzLiteCodec() = default;
    ~LzLiteCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "LZLite"; }
    uint16_t id() const override { return 6; }
};

} // namespace vde
