#include "vde/net/vde_server.h"

namespace vde {

VdeServerDaemon::VdeServerDaemon(std::string host, uint16_t port)
    : host_(std::move(host)), port_(port) {}

Status VdeServerDaemon::start() {
    running_ = true;
    return Status::Ok;
}

void VdeServerDaemon::stop() {
    running_ = false;
}

} // namespace vde
