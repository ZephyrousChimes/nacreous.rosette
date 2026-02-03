#pragma once

#include "worker_context.hpp"
#include <vector>
#include <memory>
#include <cstdint>

namespace rosette {

class Port;
class Channel;

class Scheduler {
public:
    Scheduler();
    ~Scheduler();

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    bool add_worker(uint32_t lcore_id, Port* port, uint16_t queue_id, Channel* channel);
    
    void start();
    void stop();
    
    bool is_running() const { return running_; }

private:
    std::vector<WorkerContext> workers_;
    std::vector<std::unique_ptr<Channel>> channels_;
    volatile bool running_{false};
};

int worker_main(void* arg);

} // namespace rosette
