/**
 * Copyright (c) 2022
 * The lfu eviction cache:
 * Some useful references:
 * http://dhruvbird.com/lfu.pdf
 * https://github.com/apache/activemq/blob/main/activemq-kahadb-store/src/main/java/org/apache/activemq/util/LFUCache.java
 * https://gist.github.com/cgxxv/b07cd654e6f63a0c9e861fadb06d923a
 *
 * The insertion sort algorithm analysis:
 * https://jakhi.com/sorted-doubly-linked-list-time-complexity
 */

#pragma once
#include <sys/time.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "cache.hpp"
#include "list.hpp"

namespace mcache {
template <class K, class V>
class lfuItem : public cacheItem<K, V> {
   public:
    using cacheItem<K, V>::get_value;
    using cacheItem<K, V>::put_value;
    using cacheItem<K, V>::expire;
    using cacheItem<K, V>::is_expired;

    const K key;
    Element<lfuItem<K, V> *> *element;
    std::size_t *freq;
    struct timeval *accessed_at;

    lfuItem(const K _key, const V _value)
        : cacheItem<K, V>(_value), key(_key), element(nullptr), freq(nullptr), accessed_at(nullptr) {}
};

template <class K, class V>
class LFU : public Cache<K, V> {
   public:
    LFU(std::size_t _max_cap) : Cache<K, V>(_max_cap) {}

    std::size_t Put(const K &key, const V &value) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            Evict(1);
            auto item = std::make_unique<lfuItem<K, V>>(key, value);
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

        update(found->second.get());

        if (found->second->is_expired()) {
            if (found->second->element != nullptr) {
                remove(found->second->element);
            }
            items.erase(found);
            throw std::range_error("No such element in the cache");
        }

        return found->second->get_value();
    }

    bool Has(const K &key) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            return false;
        }

        update(found->second.get());

        if (found->second->is_expired()) {
            if (found->second->element != nullptr) {
                remove(found->second->element);
            }
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
        if (found->second->element != nullptr) {
            remove(found->second->element);
        }
        items.erase(found);
        return true;
    }

    bool Evict(const int count) noexcept override {
        if (Size() < max_cap) return false;

        auto cnt = 0;
        auto *e = _list.front();
        while (e != _list.root()) {
            if (cnt >= count) {
                break;
            }
            auto *next = e->next;
            auto found = items.find(e->data->key);
            if (found != items.end()) {
                remove(e);
                items.erase(found);
                cnt++;
            }
            e = next;
        }

        return cnt == count;
    }

    std::size_t Size() override { return items.size(); }

    virtual void debug() override {
        auto *e = _list.front();
        while (e != _list.root()) {
            auto item = e->data;
            std::cout << "[" << CACHE_LFU << "] " << max_cap << "/" << Size()
                      << ", key: " << item->key
                      << ", value: " << item->get_value()
                      << ", freq: " << *item->freq << std::endl;
            e = e->next;
        }
    }

   protected:
    List<lfuItem<K, V> *> _list;
    std::unordered_map<K, std::unique_ptr<lfuItem<K, V>>> items;
    using Cache<K, V>::max_cap;

    virtual void remove(Element<lfuItem<K, V> *> *e) {
        _list.remove(e);
        delete e;
    }

    // TODO: the most worse time complexity will be O(n) for the insertion sort.
    // The insertion sort may has bad performance, will be evaluated and
    // optimized in the future.
    virtual void update(lfuItem<K, V> *item) {
        if (item->freq == nullptr) {
            item->freq = new std::size_t(0);
        }
        *item->freq += 1;
        if (_list.size() == 0) {
            auto *element = new Element(item);
            item->element = element;
            _list.push_back(item->element);
        } else if (_list.size() > 0) {
            auto found = false;
            auto *e = _list.front();
            while (e != _list.root()) {
                if (*e->data->freq >= *item->freq && e->data->key != item->key) {
                    if (item->element != nullptr) {
                        _list.move_before(item->element, e);
                    } else {
                        auto *element = new Element(item);
                        item->element = element;
                        _list.insert_before(item->element, e);
                    }
                    found = true;
                    break;
                }
                e = e->next;
            }
            if (item->element == nullptr) {
                throw std::runtime_error("unreachable");
            }
            if (!found) {
                _list.move_to_back(item->element);
            }
        } else {
            throw std::range_error("unreachable");
        }
    }
};

}  // namespace mcache
