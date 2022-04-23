#pragma once
#include <string>
#include <unordered_map>

#include "options.hpp"
#include "simple.hpp"

using namespace std;

namespace mcache {
template <class K, class V>
class Cache {
   public:
    Cache();
    Cache(string type);
    ~Cache();
    int set(const K &key, const V *value, option opts...);
    const V *get(const K &key, option opts...);
    int mset(const vector<K> &keys, const vector<V> &values, option opts...);
    const unordered_map<K, const V *> mget(const vector<K> &keys,
                                           option opts...);
    bool remove(const K &key);
    int mremove(const vector<K> &keys);
    bool has(const K &key);

   private:
    Cacher<K, V> cc;
};

template <class K, class V>
Cache<K, V>::Cache() {
    cc = Simple<K, V>();
}

template <class K, class V>
Cache<K, V>::Cache(string type) {
    // switch (type) {
    //     case TYPE_LRU:
    //         cc = LRU<T>;
    //         break;
    //     case TYPE_LFU<T>:
    //         cc = LFU<T>;
    //         break;
    //     case TYPE_ARC:
    //         cc = ARC<T>;
    //         break;
    //     default:
    //         cc = Simple<T>;
    // }
}

template <class K, class V>
Cache<K, V>::~Cache() {}

template <class K, class V>
int Cache<K, V>::set(const K &key, const V *value, option opts...) {
    return cc.set(key, value);
}

template <class K, class V>
const V *Cache<K, V>::get(const K &key, option opts...) {
    return cc.get(key);
}

template <class K, class V>
int Cache<K, V>::mset(const vector<K> &keys, const vector<V> &values,
                      option opts...) {
    if (keys.size() != values.size()) {
        return -1;
    }
    int result = 0;
    for (int i = 0; i < keys.size(); i++) {
        int rt = cc.set(keys[i], values[i]);
        if (rt == 1) {
            result++;
        }
    }

    return result;
}

template <class K, class V>
const unordered_map<K, const V *> Cache<K, V>::mget(const vector<K> &keys,
                                                    option opts...) {
    unordered_map<K, const V *> result;
    for (auto key : keys) {
        const V *val = cc.get(key);
        if (val != nullptr) {
            result.insert(key, val);
        }
    }

    return result;
}

template <class K, class V>
bool Cache<K, V>::remove(const K &key) {
    return cc.remove(key);
}

template <class K, class V>
int Cache<K, V>::mremove(const vector<K> &keys) {
    int result = 0;
    for (auto key : keys) {
        if (cc.remove(key)) {
            result++;
        }
    }

    return result;
}

template <class K, class V>
bool Cache<K, V>::has(const K &key) {
    return cc.has(key);
}
}  // namespace mcache
