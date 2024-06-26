#pragma once
#include <sys/time.h>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

#include "cache.hpp"

using namespace std;

namespace mcache {

template <class K, class V>
class Demo : public Cache<K, V> {
   public:
    Demo(size_t _max_cap) : Cache<K, V>(_max_cap) {}

    size_t Put(const K &key, const V &value) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            Evict(1);
            items.emplace(key, value);
            return 1;
        }
        found->second = value;
        return 0;
    }

    const V &Get(const K &key) override {
        auto found = items.find(key);
        if (found == items.end()) {
            throw std::range_error("No such element in the cache");
        }
        return found->second;
    }

    bool Has(const K &key) noexcept override {
        return items.find(key) != items.end();
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

        int cnt = 0;
        for (auto it = items.begin(); it != items.end(); it++) {
            if (cnt >= count) {
                break;
            }
            items.erase(it);
            cnt++;
        }

        return cnt == count;
    }

    size_t Size() noexcept override {
        return items.size();
    }

    void debug() noexcept override {
        for (auto it = items.begin(); it != items.end(); it++) {
            std::cout << "key: " << it->first <<
                        ", val: " << it->second << std::endl;
        }
    }

   private:
    unordered_map<K, V> items;
    using Cache<K, V>::max_cap;
};

}  // namespace mcache
