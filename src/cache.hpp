#pragma once
#include <sys/time.h>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iostream>

#include "list.hpp"

namespace mcache {

#define CACHE_SIMPLE "SIMPLE"
#define CACHE_LFU "LFU"
#define CACHE_LRU "LRU"
#define CACHE_FIFO "FIFO"

int ttl(const struct timeval &expire) noexcept {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    int t = expire.tv_sec - tv.tv_sec;
    if (t < 0) {
        return -1;
    }

    return t;
}

template <class K, class V>
class cacheItem {
   protected:
    V value;
    struct timeval _expire;  // time to be expired, `now + ttl`
    std::size_t _ttl;

    //TODO: ttl should be dealed with dynamic options
    cacheItem(const V _value) : value(_value), _ttl(3600) {
        gettimeofday(&_expire, nullptr);
        _expire.tv_sec += _ttl;
    }
    ~cacheItem() = default;

    bool is_expired() {
        int ts = ttl(_expire);
        if (ts < 0) {
            return true;
        }

        return false;
    }

    const V &get_value() noexcept { return value; }
    void put_value(const V _value) noexcept { value = _value; }
    const struct timeval &expire() noexcept { return _expire; }
};

template <class K, class V>
class Cache {
   protected:
    Cache(std::size_t _max_cap) : max_cap(_max_cap) {}
    virtual ~Cache() = default;

    virtual std::size_t Put(const K &key, const V &value) = 0;
    virtual const V &Get(const K &key) = 0;
    virtual bool Has(const K &key) = 0;
    virtual bool Remove(const K &key) = 0;
    virtual void Evict(const int count) = 0;
    virtual std::size_t Size() = 0;
    virtual void debug() = 0;

    std::size_t max_cap;
};

}