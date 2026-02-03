#pragma once

#include "channel.hpp"
#include <cstdint>

namespace rosette {

class Port;

class UnorderedChannel : public Channel {
public:
    explicit UnorderedChannel(Port* tx_port);
    ~UnorderedChannel() override = default;

    void on_receive(Packet& pkt) override;
    void send(Packet&& pkt) override;

private:
    Port* tx_port_;
};

} // namespace rosette
