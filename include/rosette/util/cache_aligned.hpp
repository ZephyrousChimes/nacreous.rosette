#pragma once

#include <cstddef>

namespace rosette {

constexpr size_t CACHE_LINE_SIZE = 64;

template<typename T>
struct alignas(CACHE_LINE_SIZE) CacheAligned {
    T value;
    
    CacheAligned() = default;
    explicit CacheAligned(const T& v) : value(v) {}
    
    operator T&() { return value; }
    operator const T&() const { return value; }
    
    T& operator=(const T& v) {
        value = v;
        return value;
    }
};

} // namespace rosette
