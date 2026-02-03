#pragma once

#include <cstdint>
#include <memory>

struct rte_mbuf;

namespace rosette {

class Mempool;

class Port {
public:
    Port(uint16_t port_id, Mempool* mempool);
    ~Port();

    Port(const Port&) = delete;
    Port& operator=(const Port&) = delete;

    bool configure(uint16_t nb_rx_queues, uint16_t nb_tx_queues);
    bool setup_rx_queue(uint16_t queue_id, uint16_t nb_rx_desc);
    bool setup_tx_queue(uint16_t queue_id, uint16_t nb_tx_desc);
    bool start();
    void stop();

    uint16_t rx_burst(uint16_t queue_id, rte_mbuf** rx_pkts, uint16_t nb_pkts);
    uint16_t tx_burst(uint16_t queue_id, rte_mbuf** tx_pkts, uint16_t nb_pkts);

    uint16_t port_id() const { return port_id_; }
    int socket_id() const { return socket_id_; }

private:
    uint16_t port_id_;
    int socket_id_;
    Mempool* mempool_;
    bool started_{false};
};

} // namespace rosette
