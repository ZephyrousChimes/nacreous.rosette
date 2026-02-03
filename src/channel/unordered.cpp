#include "rosette/channel/unordered.hpp"
#include "rosette/io/port.hpp"
#include "rosette/util/metrics.hpp"

#include <rte_branch_prediction.h>
#include <rte_mbuf.h>

namespace rosette {

UnorderedChannel::UnorderedChannel(Port* tx_port)
    : tx_port_(tx_port)
{
}

void UnorderedChannel::on_receive(Packet& pkt)
{
    if (unlikely(!pkt.valid())) {
        return;
    }

    rte_mbuf* mbuf = pkt.release();
    auto& metrics  = MetricsCollector::instance().get_metrics();

    const uint16_t sent = tx_port_->tx_burst(0, &mbuf, 1);

    if (unlikely(sent == 0)) {
        rte_pktmbuf_free(mbuf);
        metrics.drops.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    metrics.tx_packets.fetch_add(1, std::memory_order_relaxed);
    metrics.tx_bytes.fetch_add(
        rte_pktmbuf_pkt_len(mbuf),
        std::memory_order_relaxed
    );
}

void UnorderedChannel::send(Packet&& pkt)
{
    on_receive(pkt);
}

} // namespace rosette
