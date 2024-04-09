#pragma once
#include <sys/time.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "cache.hpp"

namespace mcache {
template <class K, class V>
class simpleItem : public cacheItem<K, V> {
   public:
    using cacheItem<K, V>::get_value;
    using cacheItem<K, V>::put_value;
    using cacheItem<K, V>::expire;
    using cacheItem<K, V>::is_expired;

    std::size_t index;
    simpleItem(const V _value) : cacheItem<K, V>(_value), index(0) {}
};

template <class K, class V>
class Simple : public Cache<K, V> {
   public:
    Simple(std::size_t _max_cap) : Cache<K, V>(_max_cap) {}
    // TODO: clean up
    ~Simple() {}

    std::size_t Put(const K &key, const V &value) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            Evict(1);
            // Why use unique_ptr is for three reasons:
            // 1. For auto gc.
            // 2. As it is only owned by items.
            // 3. As unique_ptr has the best performance.
            auto item = std::make_unique<simpleItem<K, V>>(value);
            items.emplace(key, std::move(item));
            return 1;
        }
        found->second->put_value(value);
        return 0;
    }

    const V &Get(const K &key) override {
        auto found = items.find(key);
        if (found == items.end()) {
            throw std::range_error("No such element in the cache");
        }

        if (found->second->is_expired()) {
            items.erase(found);
            throw std::range_error("Element has been expired");
        }

        return found->second->get_value();
    }

    bool Has(const K &key) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            return false;
        }
        if (found->second->is_expired()) {
            items.erase(found);
            return false;
        }

        return true;
    }

    bool Remove(const K &key) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            return false;
        }
        items.erase(found);
        return true;
    }

    bool Evict(const int count) noexcept override {
        if (Size() < max_cap) return false;

        auto cnt = 0;
        for (auto it = items.begin(); it != items.end(); it++) {
            if (cnt == count) {
                break;
            }
            items.erase(it);
            cnt++;
        }

        return cnt == count;
    }

    std::size_t Size() noexcept override { return items.size(); }

    void debug() noexcept override {
        for (auto it = items.begin(); it != items.end(); it++) {
            std::cout << "[" << CACHE_SIMPLE << "] " << max_cap << "/" << Size()
                      << ", key: " << it->first
                      << ", value: " << it->second->get_value() << ", expire: ("
                      << it->second->expire().tv_sec << ", "
                      << it->second->expire().tv_usec << ")"
                      << ", index: " << it->second->index << std::endl;
        }
    }

   private:
    std::unordered_map<K, std::unique_ptr<simpleItem<K, V>>> items;
    using Cache<K, V>::max_cap;
};

}  // namespace mcache
