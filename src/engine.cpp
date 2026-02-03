#include "rosette/engine.hpp"
#include "rosette/scheduler/scheduler.hpp"
#include "rosette/io/port.hpp"
#include "rosette/io/mempool.hpp"
#include "rosette/channel/unordered.hpp"
#include "rosette/util/metrics.hpp"

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>

#include <stdexcept>

#ifdef ROSETTE_DEBUG
#include <iostream>
#define ROSETTE_LOG(x) \
    do { std::cerr << "[ENGINE] " << x << "\n"; } while (0)
#else
#define ROSETTE_LOG(x) do {} while (0)
#endif

namespace rosette {

Engine::Engine() = default;
Engine::~Engine() = default;

bool Engine::init(int argc, char** argv)
{
    if (rte_eal_init(argc, argv) < 0) {
        return false;
    }

    const uint16_t nb_ports = rte_eth_dev_count_avail();
    if (nb_ports == 0) {
        return false;
    }

    scheduler_ = std::make_unique<Scheduler>();

    uint16_t port_id;
    RTE_ETH_FOREACH_DEV(port_id) {

        int socket_id = rte_eth_dev_socket_id(port_id);
        if (socket_id < 0) {
            socket_id = 0;
        }

        auto mempool = std::make_unique<Mempool>(
            "mbuf_pool_" + std::to_string(port_id),
            8192,
            256,
            2048,
            socket_id
        );

        auto port = std::make_unique<Port>(port_id, mempool.get());

        if (!port->configure(1, 1)) {
            return false;
        }

        if (!port->setup_rx_queue(0, 1024)) {
            return false;
        }

        if (!port->setup_tx_queue(0, 1024)) {
            return false;
        }

        if (!port->start()) {
            return false;
        }

        uint32_t worker_lcore = UINT32_MAX;
        uint32_t lcore_id;

        RTE_LCORE_FOREACH_WORKER(lcore_id) {
            if (rte_lcore_to_socket_id(lcore_id) ==
                static_cast<uint32_t>(socket_id)) {
                worker_lcore = lcore_id;
                break;
            }
        }

        if (worker_lcore == UINT32_MAX) {
            RTE_LCORE_FOREACH_WORKER(lcore_id) {
                worker_lcore = lcore_id;
                break;
            }
        }

        if (worker_lcore == UINT32_MAX) {
            return false;
        }

        if (rte_lcore_to_socket_id(worker_lcore) !=
            static_cast<uint32_t>(socket_id)) {
            ROSETTE_LOG("NUMA mismatch: port "
                        << port_id << " socket "
                        << socket_id << " -> lcore "
                        << worker_lcore);
        }

        auto channel = std::make_unique<UnorderedChannel>(port.get());

        if (!scheduler_->add_worker(worker_lcore,
                                    port.get(),
                                    0,
                                    channel.get())) {
            return false;
        }

        mempools_.push_back(std::move(mempool));
        ports_.push_back(std::move(port));
        channels_.push_back(std::move(channel));
    }

    return true;
}

void Engine::start()
{
    if (running_) {
        return;
    }

    running_ = true;
    scheduler_->start();
}

void Engine::stop()
{
    if (!running_) {
        return;
    }

    scheduler_->stop();

    for (auto& port : ports_) {
        port->stop();
    }

    running_ = false;

    MetricsCollector::instance().print_metrics();
}

} // namespace rosette
