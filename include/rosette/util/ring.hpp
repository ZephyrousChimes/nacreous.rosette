#pragma once

#include "cache_aligned.hpp"
#include <atomic>
#include <cstddef>
#include <optional>

namespace rosette {

template<typename T, size_t Capacity>
class SPSCRing {
public:
    SPSCRing() : head_(0), tail_(0) {
        static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    }

    ~SPSCRing() {
        while (pop()) {}
    }

    SPSCRing(const SPSCRing&) = delete;
    SPSCRing& operator=(const SPSCRing&) = delete;

    bool push(const T& item) {
        const size_t current_tail = tail_.value.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & (Capacity - 1);
        
        if (next_tail == head_.value.load(std::memory_order_acquire)) {
            return false; // Full
        }
        
        buffer_[current_tail] = item;
        tail_.value.store(next_tail, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        const size_t current_head = head_.value.load(std::memory_order_relaxed);
        
        if (current_head == tail_.value.load(std::memory_order_acquire)) {
            return std::nullopt; // Empty
        }
        
        T item = buffer_[current_head];
        head_.value.store((current_head + 1) & (Capacity - 1), std::memory_order_release);
        return item;
    }

    bool empty() const {
        return head_.value.load(std::memory_order_acquire) == 
               tail_.value.load(std::memory_order_acquire);
    }

    size_t size() const {
        const size_t h = head_.value.load(std::memory_order_acquire);
        const size_t t = tail_.value.load(std::memory_order_acquire);
        return (t - h) & (Capacity - 1);
    }

private:
    CacheAligned<std::atomic<size_t>> head_;
    CacheAligned<std::atomic<size_t>> tail_;
    T buffer_[Capacity];
};

} // namespace rosette
