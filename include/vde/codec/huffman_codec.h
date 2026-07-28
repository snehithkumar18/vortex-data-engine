#pragma once

#include "vde/codec/codec.h"
#include <functional>
#include <vector>

namespace vde {

struct HuffmanNode {
    HuffmanNode* left = nullptr;
    HuffmanNode* right = nullptr;
    int16_t symbol = -1;
    uint32_t freq = 0;

    bool is_leaf() const { return left == nullptr && right == nullptr; }
};

class HuffmanCodec : public ICodec {
public:
    HuffmanCodec();
    ~HuffmanCodec() override;

    Status decompress(Span<const byte_t> input, OwnedBuffer* output) override;
    Status compress(Span<const byte_t> input, OwnedBuffer* output) override;

    using PruneCallback = std::function<void(HuffmanNode* pruned_node)>;
    void set_prune_callback(PruneCallback cb) { prune_cb_ = std::move(cb); }

    const char* name() const override { return "Huffman"; }
    uint16_t id() const override { return static_cast<uint16_t>(CodecId::Huffman); }

private:
    void free_tree(HuffmanNode* node);
    void prune_zero_freq(HuffmanNode* node);

    HuffmanNode* root_ = nullptr;
    std::vector<HuffmanNode*> node_pool_;
    uint32_t* freq_table_ = nullptr;
    PruneCallback prune_cb_;
};

}
