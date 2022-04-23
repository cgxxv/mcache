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

    size_t Set(const K &key, const V &value) noexcept override {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        auto found = items.find(key);
        if (found == items.end()) {
            if (items.size() >= max_cap) {
                Evict(1);
            }
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

    void Evict(const int count) noexcept override {
        int cnt = 0;
        for (auto it = items.begin(); it != items.end(); it++) {
            if (cnt == count) {
                break;
            }
            items.erase(it);
            cnt++;
        }

        return;
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
    size_t max_cap;
};

}  // namespace mcache
