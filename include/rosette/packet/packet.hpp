#pragma once

#include <cstdint>
#include <cstddef>

struct rte_mbuf;

namespace rosette {

class Packet {
public:
    Packet() : mbuf_(nullptr) {}
    explicit Packet(rte_mbuf* mbuf) : mbuf_(mbuf) {}
    
    Packet(Packet&& other) noexcept : mbuf_(other.mbuf_) {
        other.mbuf_ = nullptr;
    }
    
    Packet& operator=(Packet&& other) noexcept {
        if (this != &other) {
            release();
            mbuf_ = other.mbuf_;
            other.mbuf_ = nullptr;
        }
        return *this;
    }
    
    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;
    
    ~Packet();

    uint8_t* data();
    const uint8_t* data() const;
    
    size_t size() const;
    
    rte_mbuf* mbuf() { return mbuf_; }
    const rte_mbuf* mbuf() const { return mbuf_; }
    
    rte_mbuf* release() {
        rte_mbuf* m = mbuf_;
        mbuf_ = nullptr;
        return m;
    }
    
    bool valid() const { return mbuf_ != nullptr; }

private:
    rte_mbuf* mbuf_;
};

} // namespace rosette
