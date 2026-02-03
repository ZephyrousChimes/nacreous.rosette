#include "rosette/packet/packet.hpp"
#include <rte_mbuf.h>

namespace rosette {

Packet::~Packet() {
    if (mbuf_) {
        rte_pktmbuf_free(mbuf_);
    }
}

uint8_t* Packet::data() {
    return rte_pktmbuf_mtod(mbuf_, uint8_t*);
}

const uint8_t* Packet::data() const {
    return rte_pktmbuf_mtod(mbuf_, const uint8_t*);
}

size_t Packet::size() const {
    return rte_pktmbuf_pkt_len(mbuf_);
}

} // namespace rosette
