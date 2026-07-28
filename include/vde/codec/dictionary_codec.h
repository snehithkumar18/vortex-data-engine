#pragma once

#include "vde/codec/codec.h"
#include <vector>
#include <string>

namespace vde {

class DictionaryCodec : public ICodec {
public:
    DictionaryCodec() = default;
    ~DictionaryCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "Dictionary"; }
    uint16_t id() const override { return 10; }
};

}
