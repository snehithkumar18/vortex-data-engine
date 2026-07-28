#pragma once

#include "vde/codec/codec.h"

namespace vde {

class ForCodec : public ICodec {
public:
    ForCodec() = default;
    ~ForCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "FOR"; }
    uint16_t id() const override { return 5; }
};

} // namespace vde
