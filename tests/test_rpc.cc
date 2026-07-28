#include "tests/test_framework.h"
#include "vde/net/rpc_channel.h"

TEST(rpc_channel_register_process) {
    vde::RpcChannel rpc;
    rpc.register_handler(1, [](vde::Span<const vde::byte_t> payload) {
        (void)payload;
        vde::ByteWriter w;
        w.write_u32_le(0x1234);
        return w.release();
    });

    vde::ByteWriter req;
    req.write_u32_le(0x01435052);
    req.write_u16_le(1);
    req.write_u32_le(100);
    req.write_u32_le(4);
    req.write_u32_le(0x9999);

    vde::OwnedBuffer req_buf = req.release();
    vde::OwnedBuffer resp_buf = rpc.process_incoming_raw(req_buf.span());
    ASSERT_NE(resp_buf.size(), 0u);
}

RUN_ALL_TESTS()
