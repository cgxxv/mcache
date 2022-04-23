#pragma once
#include <string>
#include <unordered_map>

#include "options.hpp"
#include "simple.hpp"
#include "fifo.hpp"
#include "lru.hpp"
#include "lfu.hpp"

namespace mcache {
template <class K, class V>
class MCache {
   public:
    MCache(std::size_t _max_cap) : cc(Simple<K, V>(_max_cap)) {}

    MCache(std::size_t _max_cap, std::size_t type) {
        switch (type) {
        case TYPE_LRU:
            cc = LRU<K, V>(_max_cap);
            break;
        case TYPE_LFU:
            cc = LFU<K, V>(_max_cap);
            break;
        case TYPE_FIFO:
            cc = FIFO<K, V>(_max_cap);
            break;
        default:
            cc = Simple<K, V>(_max_cap);
        }
    }

    ~MCache() = default;

    std::size_t Set(const K &key, const V &value, option opts...) {
        return cc.Set(key, value);
    }

    const V &Get(const K &key, option opts...) {
        return cc.Get(key);
    }

    bool Remove(const K &key) {
        return cc.Remove(key);
    }

    std::size_t MSet(const vector<K> &keys, const vector<V> &values, option opts...) {
        if (keys.size() != values.size()) {
            throw std::range_error("Unmatched key and value");
        }

        auto cnt = 0;
        for (size_t i = 0; i < keys.size(); i++)
        {
            auto ret = cc.Set(keys[i], values[i]);
            if (ret == 1) {
                cnt++;
            }
        }

        return cnt;
    }

    std::unordered_map<K, V> MGet(const vector<K> &keys, option opts...) {
        std::unordered_map<K, V> result;
        for (auto key : keys) {
            const V *val = cc.Get(key);
            if (val != nullptr) {
                result.emplace(key, val);
            }
        }

        return result;
    }

    std::size_t MRemove(const vector<K> &keys) {
        auto cnt = 0;
        for (auto key : keys) {
            if (cc.Remove(key)) {
                cnt++;
            }
        }

        return cnt;
    }

    bool Has(const K &key) {
        return cc.Has(key);
    }

    std::size_t Size() {
        return cc.Size();
    }

    void debug() {
        return cc.debug();
    }

   private:
    Cacher<K, V> cc;
};

}
