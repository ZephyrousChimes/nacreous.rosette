#include "rosette/util/metrics.hpp"
#include <iostream>
#include <iomanip>

namespace rosette {

void Metrics::reset() {
    rx_packets.store(0, std::memory_order_relaxed);
    tx_packets.store(0, std::memory_order_relaxed);
    rx_bytes.store(0, std::memory_order_relaxed);
    tx_bytes.store(0, std::memory_order_relaxed);
    drops.store(0, std::memory_order_relaxed);
}

void Metrics::print() const {
    uint64_t rx_pkts = rx_packets.load(std::memory_order_relaxed);
    uint64_t tx_pkts = tx_packets.load(std::memory_order_relaxed);
    uint64_t rx_b = rx_bytes.load(std::memory_order_relaxed);
    uint64_t tx_b = tx_bytes.load(std::memory_order_relaxed);
    uint64_t d = drops.load(std::memory_order_relaxed);

    std::cout << "\n=== Metrics ===" << std::endl;
    std::cout << "RX: " << rx_pkts << " packets, " << rx_b << " bytes" << std::endl;
    std::cout << "TX: " << tx_pkts << " packets, " << tx_b << " bytes" << std::endl;
    std::cout << "Drops: " << d << std::endl;
    std::cout << "===============\n" << std::endl;
}

MetricsCollector& MetricsCollector::instance() {
    static MetricsCollector collector;
    return collector;
}

void MetricsCollector::print_metrics() const {
    metrics_.print();
}

void MetricsCollector::reset_metrics() {
    metrics_.reset();
}

} // namespace rosette
