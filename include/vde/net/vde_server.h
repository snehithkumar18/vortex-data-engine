#pragma once

#include "vde/common/types.h"
#include "vde/net/rpc_channel.h"
#include <string>
#include <vector>

namespace vde {

class VdeServerDaemon {
public:
    explicit VdeServerDaemon(std::string host = "127.0.0.1", uint16_t port = 9090);
    ~VdeServerDaemon() = default;

    Status start();
    void stop();

    bool is_running() const { return running_; }
    uint16_t port() const { return port_; }

    RpcChannel& rpc_channel() { return rpc_; }

private:
    std::string host_;
    uint16_t port_;
    bool running_ = false;
    RpcChannel rpc_;
};

} // namespace vde
