#pragma once

#include "../packet/packet.hpp"

namespace rosette {

class Channel {
public:
    virtual ~Channel() = default;
    
    virtual void on_receive(Packet& pkt) = 0;
    virtual void send(Packet&& pkt) = 0;
    
    virtual void tick() {}
};

} // namespace rosette
