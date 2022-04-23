#pragma once
#include <sys/time.h>

namespace mcache {

#define MAX_INSERT_SORT_SIZE 512 //when cache is less than 512, we use insertion sort

#define MCACHE_ERROR_ENUM(x)                                  \
    x(not_found,          "No such element in the cache")     \
    x(broken_request,     "Request packet is broken")

// bool set(const K &key, const void *value);
// int mset(const vector<string> &keys, const vector<void> *values);

// const void *get(const K &key);
// const vector<void> mget(const vector<string> &keys);

// bool remove(const K &key);
// int mremove(const vector<string> &keys);
// bool has(const K &key);

// the remote cache client, eg: memcache client, redis client
template <class K, class V>
class Cacher {
   public:
    // virtual Cacher(size_t max_cap) = default;
    virtual ~Cacher() = default;
    virtual std::size_t Set(const K &key, const V &value) = 0;
    virtual const V &Get(const K &key) = 0;
    virtual bool Has(const K &key) = 0;
    virtual bool Remove(const K &key) = 0;
    virtual void Evict(const int count) = 0;
    virtual std::size_t Size() = 0;
};

int ttl(struct timeval &expire) noexcept {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    int t = expire.tv_sec - tv.tv_sec;
    if (t < 0) {
        return -1;
    }

    return t;
}

}  // namespace mcache
