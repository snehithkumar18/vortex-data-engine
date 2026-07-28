#pragma once

#include "vde/codec/codec.h"

namespace vde {

class SnappyLiteCodec : public ICodec {
public:
    SnappyLiteCodec() = default;
    ~SnappyLiteCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "SnappyLite"; }
    uint16_t id() const override { return 7; }
};

}
