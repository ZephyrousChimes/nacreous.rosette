#include "rosette/io/port.hpp"
#include "rosette/io/mempool.hpp"

#include <rte_ethdev.h>
#include <rte_errno.h>

#include <stdexcept>

namespace rosette {

namespace {
constexpr rte_eth_conf default_port_conf{
    .rxmode = {
        .mq_mode = RTE_ETH_MQ_RX_NONE,
    },
    .txmode = {
        .mq_mode = RTE_ETH_MQ_TX_NONE,
    },
};
}

Port::Port(uint16_t port_id, Mempool* mempool)
    : port_id_(port_id),
      mempool_(mempool),
      started_(false)
{
    socket_id_ = rte_eth_dev_socket_id(port_id_);
    if (socket_id_ < 0) {
        socket_id_ = 0;
    }
}

Port::~Port()
{
    if (started_) {
        stop();
    }
}

bool Port::configure(uint16_t nb_rx_queues,
                     uint16_t nb_tx_queues)
{
    rte_eth_dev_info dev_info{};
    const int ret_info = rte_eth_dev_info_get(port_id_, &dev_info);
    if (ret_info != 0) {
        return false;
    }

    rte_eth_conf port_conf = default_port_conf;

    const int ret_cfg =
        rte_eth_dev_configure(port_id_,
                              nb_rx_queues,
                              nb_tx_queues,
                              &port_conf);

    return ret_cfg == 0;
}

bool Port::setup_rx_queue(uint16_t queue_id,
                          uint16_t nb_rx_desc)
{
    const int ret =
        rte_eth_rx_queue_setup(
            port_id_,
            queue_id,
            nb_rx_desc,
            socket_id_,
            nullptr,
            mempool_->pool());

    return ret >= 0;
}

bool Port::setup_tx_queue(uint16_t queue_id,
                          uint16_t nb_tx_desc)
{
    const int ret =
        rte_eth_tx_queue_setup(
            port_id_,
            queue_id,
            nb_tx_desc,
            socket_id_,
            nullptr);

    return ret >= 0;
}

bool Port::start()
{
    const int ret = rte_eth_dev_start(port_id_);
    if (ret < 0) {
        return false;
    }

    rte_eth_promiscuous_enable(port_id_);
    started_ = true;
    return true;
}

void Port::stop()
{
    rte_eth_dev_stop(port_id_);
    started_ = false;
}

uint16_t Port::rx_burst(uint16_t queue_id,
                        rte_mbuf** rx_pkts,
                        uint16_t nb_pkts)
{
    return rte_eth_rx_burst(port_id_,
                            queue_id,
                            rx_pkts,
                            nb_pkts);
}

uint16_t Port::tx_burst(uint16_t queue_id,
                        rte_mbuf** tx_pkts,
                        uint16_t nb_pkts)
{
    return rte_eth_tx_burst(port_id_,
                            queue_id,
                            tx_pkts,
                            nb_pkts);
}

} // namespace rosette
