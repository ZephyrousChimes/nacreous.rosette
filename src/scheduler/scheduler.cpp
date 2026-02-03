#include "rosette/scheduler/scheduler.hpp"
#include "rosette/io/port.hpp"
#include "rosette/channel/channel.hpp"

#include <rte_lcore.h>
#include <rte_launch.h>

#include <stdexcept>

namespace rosette {

Scheduler::Scheduler() = default;

Scheduler::~Scheduler()
{
    if (running_) {
        stop();
    }
}

bool Scheduler::add_worker(uint32_t lcore_id,
                           Port* port,
                           uint16_t queue_id,
                           Channel* channel)
{
    if (!rte_lcore_is_enabled(lcore_id)) {
        return false;
    }

    if (rte_lcore_to_socket_id(lcore_id) != port->socket_id()) {
        return false;
    }

    WorkerContext ctx{};
    ctx.lcore_id = lcore_id;
    ctx.queue_id = queue_id;
    ctx.port     = port;
    ctx.channel  = channel;
    ctx.running  = &running_;

    workers_.emplace_back(std::move(ctx));
    return true;
}

void Scheduler::start()
{
    if (running_) {
        return;
    }

    running_ = true;

    for (auto& worker : workers_) {
        if (rte_eal_remote_launch(worker_main,
                                  &worker,
                                  worker.lcore_id) != 0) {
            running_ = false;
            throw std::runtime_error("[SCHEDULER] Worker didn't launch");
        }
    }
}

void Scheduler::stop()
{
    if (!running_) {
        return;
    }

    running_ = false;
    rte_eal_mp_wait_lcore();
}

} // namespace rosette
