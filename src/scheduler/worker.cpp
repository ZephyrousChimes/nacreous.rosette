#include "rosette/scheduler/scheduler.hpp"
#include "rosette/scheduler/worker_context.hpp"
#include "rosette/io/port.hpp"
#include "rosette/channel/channel.hpp"
#include "rosette/packet/packet.hpp"
#include "rosette/util/metrics.hpp"

#include <rte_mbuf.h>
#include <rte_cycles.h>
#include <rte_lcore.h>

#include <iostream>

namespace rosette {

constexpr uint16_t BURST_SIZE = 32;

int worker_main(void* arg) {
    WorkerContext* ctx = static_cast<WorkerContext*>(arg);
    
    const uint32_t lcore_id = rte_lcore_id();
    std::cout << "Worker started on lcore " << lcore_id << std::endl;

    rte_mbuf* rx_burst[BURST_SIZE];
    
    uint64_t loop_count = 0;
    uint64_t last_tsc = rte_rdtsc();
    
    while (*ctx->running) {
        // Poll for packets
        uint16_t nb_rx = ctx->port->rx_burst(ctx->queue_id, rx_burst, BURST_SIZE);
        
        if (nb_rx > 0) {
            auto& metrics = MetricsCollector::instance().get_metrics();
            metrics.rx_packets.fetch_add(nb_rx, std::memory_order_relaxed);
            
            // Process each packet
            for (uint16_t i = 0; i < nb_rx; i++) {
                metrics.rx_bytes.fetch_add(
                    rte_pktmbuf_pkt_len(rx_burst[i]), 
                    std::memory_order_relaxed
                );
                
                // Wrap mbuf in Packet abstraction
                Packet pkt(rx_burst[i]);
                
                // Dispatch to channel
                ctx->channel->on_receive(pkt);
            }
        }
        
        // Periodic channel tick for housekeeping
        if (++loop_count % 1000 == 0) {
            ctx->channel->tick();
            
            // Optional: print metrics every ~1M loops
            if (loop_count % 1000000 == 0) {
                uint64_t now_tsc = rte_rdtsc();
                uint64_t elapsed = now_tsc - last_tsc;
                last_tsc = now_tsc;
                
                // Could log cycles per iteration here if needed
                (void)elapsed; // Suppress unused warning
            }
        }
    }
    
    std::cout << "Worker on lcore " << lcore_id << " shutting down" << std::endl;
    return 0;
}

} // namespace rosette
