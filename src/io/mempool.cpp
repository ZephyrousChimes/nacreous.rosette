#include "rosette/io/mempool.hpp"

#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_errno.h>
#include <rte_string_fns.h>

#include <stdexcept>

namespace rosette {

Mempool::Mempool(const std::string& name,
                 uint32_t n_mbufs,
                 uint32_t cache_size,
                 uint16_t data_room_size,
                 int socket_id)
    : socket_id_(socket_id)
{
    pool_ = rte_pktmbuf_pool_create(
        name.c_str(),
        n_mbufs,
        cache_size,
        0,                  // priv_size
        data_room_size,
        socket_id_
    );

    if (pool_ == nullptr) {
        const int err = rte_errno;
        throw std::runtime_error(
            "[MEMPOOL] rte_pktmbuf_pool_create Failed (socket=" +
            std::to_string(socket_id_) +
            "): " + std::string(rte_strerror(err))
        );
    }
}

Mempool::~Mempool()
{
    if (pool_) {
        rte_mempool_free(pool_);
        pool_ = nullptr;
    }
}

} // namespace rosette
