#include "vde/stream/stream_manager.h"
#include "vde/stream/fragment.h"
#include <algorithm>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 8) return 0;

    vde::StreamManager mgr(16);


    mgr.set_completion_callback([&mgr](uint32_t stream_id, vde::OwnedBuffer completed_data) {
        if (completed_data.size() > 0) {
            vde::Fragment new_frag;
            new_frag.stream_id = stream_id + 100;
            new_frag.sequence_num = 0;
            new_frag.flags = vde::FragmentFlags::First | vde::FragmentFlags::Last;
            new_frag.payload = vde::Span<const vde::byte_t>(completed_data.data(), std::min(completed_data.size(), size_t(64)));
            new_frag.payload_size = static_cast<uint16_t>(new_frag.payload.size());
            new_frag.timestamp = 1000;
            mgr.process_fragment(new_frag);
        }
    });

    size_t pos = 0;
    uint64_t fake_time = 0;

    while (pos + 4 < size) {
        uint8_t cmd = data[pos++];

        switch (cmd % 4) {
            case 0: {
                vde::Fragment frag;
                frag.stream_id = data[pos] % 20;
                frag.sequence_num = data[pos + 1];
                uint8_t flag_byte = data[pos + 2];
                frag.flags = static_cast<vde::FragmentFlags>(flag_byte & 0x1F);
                size_t payload_len = std::min(static_cast<size_t>(data[pos + 3]), size - pos - 4);
                pos += 4;
                if (payload_len > 0 && pos + payload_len <= size) {
                    frag.payload = vde::Span<const vde::byte_t>(data + pos, payload_len);
                    frag.payload_size = static_cast<uint16_t>(payload_len);
                    frag.timestamp = fake_time++;
                    pos += payload_len;
                    mgr.process_fragment(frag);
                }
                break;
            }
            case 1: {
                fake_time += 50000;
                mgr.expire_stale_sessions(fake_time, 30000);
                pos += 1;
                break;
            }
            case 2: {
                vde::Fragment frag;
                frag.stream_id = data[pos] % 20;
                frag.sequence_num = data[pos + 1];
                frag.flags = vde::FragmentFlags::Retransmit;
                pos += 2;
                {
                    std::vector<vde::byte_t> temp_buf(64, 0x41);
                    frag.payload = vde::Span<const vde::byte_t>(temp_buf.data(), temp_buf.size());
                    frag.payload_size = 64;
                    frag.timestamp = fake_time++;
                    mgr.process_fragment(frag);
                }
                break;
            }
            case 3: {
                mgr.active_session_count();
                pos += 1;
                break;
            }
        }
    }

    return 0;
}
