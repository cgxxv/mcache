#pragma once
#include <sys/time.h>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "cache.hpp"

namespace mcache {
template <class V>
struct simpleItem {
    const V value;
    struct timeval expire;  // time to be expired, `now + ttl`
    int index;

    simpleItem(const V _value) : value(_value), index(0) {
        gettimeofday(&expire, nullptr);
    }
    bool is_expired() {
        int ts = ttl(expire);
        if (ts < 0) {
            return true;
        }

        return false;
    }
};

template <class K, class V>
class Simple : public Cacher<K, V> {
   public:
    Simple(std::size_t _max_cap) : max_cap(_max_cap) {}

    std::size_t Set(const K &key, const V &value) noexcept override {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        //TODO: simpleItem: cannot be assigned because its copy assignment operator is implicitly deleted
        // simpleItem item(value);
        // auto item = new simpleItem(value);
        auto item = std::make_shared<simpleItem<V>>(value);
        auto found = items.find(key);
        if (found == items.end()) {
            if (Size() > max_cap) {
                Evict(1);
            }
            items.emplace(key, item);
            return 1;
        }
        found->second = item;
        return 0;
    }

    const V &Get(const K &key) override {
        auto found = items.find(key);
        if (found == items.end()) {
            throw std::range_error("No such element in the cache");
        }
        return found->second->value;
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

    // TODO: with bad performance
    void Evict(const int count) noexcept override{
        auto cnt = 0;
        for (auto it = items.begin(); it != items.end(); it++) {
            if (cnt == count) {
                break;
            }
            items.erase(it);
            cnt++;
        }

        return;
    }

    std::size_t Size() noexcept override {
        return items.size();
    }

   private:
    std::unordered_map<K, std::shared_ptr<simpleItem<V>>> items;
    std::size_t max_cap;
};

}
