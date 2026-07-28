#include "vde/codec/huffman_codec.h"
#include "vde/common/byte_reader.h"
#include "vde/common/byte_writer.h"
#include <cstdlib>

namespace vde {

HuffmanCodec::HuffmanCodec() = default;

HuffmanCodec::~HuffmanCodec() {
    free_tree(root_);
    root_ = nullptr;

    if (freq_table_) {
        std::free(freq_table_);
        freq_table_ = nullptr;
    }
}

void HuffmanCodec::free_tree(HuffmanNode* node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    delete node;
}

void HuffmanCodec::prune_zero_freq(HuffmanNode* node) {
    if (!node) return;

    if (node->left && node->left->freq == 0) {
        if (prune_cb_) {


            prune_cb_(node->left);
        }
        delete node->left;
        node->left = nullptr;
    }

    if (node->right && node->right->freq == 0) {
        if (prune_cb_) {
            prune_cb_(node->right);
        }
        delete node->right;
        node->right = nullptr;
    }

    prune_zero_freq(node->left);
    prune_zero_freq(node->right);
}

Status HuffmanCodec::decompress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    if (input.empty()) return Status::Ok;

    ByteReader reader(input);


    if (!freq_table_) {
        freq_table_ = static_cast<uint32_t*>(std::malloc(256 * sizeof(uint32_t)));
    }
    for (int i = 0; i < 256; ++i) {
        auto f = reader.read_u16_le();
        if (!f.has_value()) return Status::Truncated;
        freq_table_[i] = f.value;
    }


    root_ = new HuffmanNode();
    for (int i = 0; i < 256; ++i) {
        if (freq_table_[i] > 0) {
            HuffmanNode* leaf = new HuffmanNode();
            leaf->symbol = static_cast<int16_t>(i);
            leaf->freq = freq_table_[i];
            node_pool_.push_back(leaf);

            if (!root_->left) root_->left = leaf;
            else if (!root_->right) root_->right = leaf;
        }
    }


    prune_zero_freq(root_);


    output->clear();
    while (reader.remaining() > 0) {
        auto b = reader.read_u8();
        if (!b.has_value()) break;
        output->append(&b.value, 1);
    }

    return Status::Ok;
}

Status HuffmanCodec::compress(Span<const byte_t> input, OwnedBuffer* output) {
    if (!output) return Status::InvalidArgument;
    output->clear();

    if (!freq_table_) {
        freq_table_ = static_cast<uint32_t*>(std::malloc(256 * sizeof(uint32_t)));
    }
    std::memset(freq_table_, 0, 256 * sizeof(uint32_t));

    for (size_t i = 0; i < input.size(); ++i) {
        freq_table_[input[i]]++;
    }

    if (input.size() > kMaxRecordSize) {


        std::free(freq_table_);
        return Status::Overflow;
    }

    ByteWriter writer;
    for (int i = 0; i < 256; ++i) {
        writer.write_u16_le(static_cast<uint16_t>(freq_table_[i]));
    }
    writer.write_bytes(input);

    *output = writer.release();
    return Status::Ok;
}

}
