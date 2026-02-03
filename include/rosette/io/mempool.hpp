#pragma once

#include <cstdint>
#include <string>

struct rte_mempool;

namespace rosette {

class Mempool {
public:
    Mempool(const std::string& name, uint32_t n_mbufs, uint32_t cache_size, 
            uint16_t data_room_size, int socket_id);
    ~Mempool();

    Mempool(const Mempool&) = delete;
    Mempool& operator=(const Mempool&) = delete;

    rte_mempool* pool() { return pool_; }
    int socket_id() const { return socket_id_; }

private:
    rte_mempool* pool_;
    int socket_id_;
};

} // namespace rosette
