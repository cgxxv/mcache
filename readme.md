# mcache

MCache is a local memory cache with some modern features, implemented LFU, LRU, FIFO, ARC, simple eviction.

## examples

```bash
mkdir build
cd build
#see more details
#cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
make simple && ./simple
make cache && ./cache
```

## Usage

### the mcache way with your dynamic options

```c++
template <class K, class V, template <class, class> class T>
class MCache : public options {
   public:
    MCache(std::size_t _max_cap) : cc(T<K, V>(_max_cap)) {}
    ~MCache() = default;

    // Put will update or add a item into the cache.
    std::size_t Put(const K &key, const V &value, option opt, ...);

    // Get return a value from the cache by the key.
    const V &Get(const K &key, option opt, ...);

    // Remove will delete a item from the cache immediately.
    bool Remove(const K &key);

    // MPut will update or add a bunch of KVs to the cache
    std::size_t MPut(const std::vector<K> &keys, const std::vector<V> &values,
                     option opt, ...);

    // MGet will return unordered_map by the keys
    std::unordered_map<K, V> MGet(const std::vector<K> &keys, option opt, ...);

    // MRemove will remove a bunch of cache items by keys.
    std::size_t MRemove(const std::vector<K> &keys);

    // Has check if a key exists
    bool Has(const K &key);
    // Evict will delete a item from the cache when the cache is full.
    bool Evict(const int count);
    // Size return the length of current cache.
    std::size_t Size();
    void debug();

   private:
    T<K, V> cc;
};
```

### the separate way

```c++
template <class K, class V>
class Cache {
   protected:
    Cache(std::size_t _max_cap) : max_cap(_max_cap) {}
    virtual ~Cache() = default;

    // Put for update the item or add a item
    // If the cache is full, and you still put a item, in this case
    // first evict a cache item, then add a the new one.
    virtual std::size_t Put(const K &key, const V &value) = 0;
    // Get for query a item by key
    virtual const V &Get(const K &key) = 0;
    // Has for checking a item exists or not
    virtual bool Has(const K &key) = 0;
    // Remove means just remove the item by key immediately
    virtual bool Remove(const K &key) = 0;
    // Evict means evict a item when the cache is full
    virtual bool Evict(const int count) = 0;
    // Size return the cache length.
    virtual std::size_t Size() = 0;
    virtual void debug() = 0;

    // max_cap is the cache capacity
    std::size_t max_cap;
};
```
