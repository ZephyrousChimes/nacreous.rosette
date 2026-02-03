#pragma once

#include <cstdint>
#include <memory>

namespace rosette {

class Port;
class Channel;

struct WorkerContext {
    uint32_t lcore_id;
    uint16_t queue_id;
    Port* port;
    Channel* channel;
    volatile bool* running;
};

} // namespace rosette
