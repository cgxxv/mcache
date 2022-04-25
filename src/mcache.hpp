#pragma once
#include <string>
#include <unordered_map>

#include "cache.hpp"
#include "fifo.hpp"
#include "lfu.hpp"
#include "lru.hpp"
#include "options.hpp"
#include "simple.hpp"

namespace mcache {
template <class K, class V, template <class, class> class T>
class MCache : public options {
   public:
    MCache(std::size_t _max_cap) : cc(T<K, V>(_max_cap)) {}
    ~MCache() = default;

    std::size_t Put(const K &key, const V &value, option opt, ...) {
        return cc.Put(key, value);
    }

    const V &Get(const K &key, option opt, ...) { return cc.Get(key); }

    bool Remove(const K &key) { return cc.Remove(key); }

    std::size_t MPut(const std::vector<K> &keys, const std::vector<V> &values,
                     option opt, ...) {
        if (keys.size() != values.size()) {
            throw std::range_error("Unmatched key and value");
        }

        auto cnt = 0;
        for (size_t i = 0; i < keys.size(); i++) {
            auto ret = cc.Set(keys[i], values[i]);
            if (ret == 1) {
                cnt++;
            }
        }

        return cnt;
    }

    std::unordered_map<K, V> MGet(const std::vector<K> &keys, option opt, ...) {
        std::unordered_map<K, V> result;
        for (auto key : keys) {
            const V *val = cc.Get(key);
            if (val != nullptr) {
                result.emplace(key, val);
            }
        }

        return result;
    }

    std::size_t MRemove(const std::vector<K> &keys) {
        auto cnt = 0;
        for (auto key : keys) {
            if (cc.Remove(key)) {
                cnt++;
            }
        }

        return cnt;
    }

    bool Has(const K &key) { return cc.Has(key); }
    void Evict(const int count) { return cc.Evict(count); }
    std::size_t Size() { return cc.Size(); }
    void debug() { return cc.debug(); }

   private:
    T<K, V> cc;
};

}  // namespace mcache
