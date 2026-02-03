#pragma once

#include <cstdint>
#include <atomic>

namespace rosette {

struct Metrics {
    std::atomic<uint64_t> rx_packets{0};
    std::atomic<uint64_t> tx_packets{0};
    std::atomic<uint64_t> rx_bytes{0};
    std::atomic<uint64_t> tx_bytes{0};
    std::atomic<uint64_t> drops{0};
    
    void reset();
    void print() const;
};

class MetricsCollector {
public:
    static MetricsCollector& instance();
    
    Metrics& get_metrics() { return metrics_; }
    const Metrics& get_metrics() const { return metrics_; }
    
    void print_metrics() const;
    void reset_metrics();

private:
    MetricsCollector() = default;
    Metrics metrics_;
};

} // namespace rosette
