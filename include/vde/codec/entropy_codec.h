#pragma once

#include "vde/codec/codec.h"
#include <vector>

namespace vde {

struct AnsSymbolTable {
    uint16_t freq[256];
    uint16_t cum_freq[257];
    uint16_t total_freq;
};

class EntropyCodec : public ICodec {
public:
    EntropyCodec() = default;
    ~EntropyCodec() override = default;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    const char* name() const override { return "EntropyANS"; }
    uint16_t id() const override { return 9; }

private:
    AnsSymbolTable table_{};
};

} // namespace vde
