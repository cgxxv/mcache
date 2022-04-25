/**
 * Copyright (c) 2022
 * LRU has high performance, and it is a absolute O(1) time complexity
 * for read and write as it it simplily O(1) double-linked list updation.
 * But as it is not that good for some data with cycle rule.
 * eg: 1，1，1，2，2，2，3，4，1，1，1，2，2，2.....
 * When read at 3, 4, then 1, 2 will be evicted.
 * But the real suition is that 1, 2 are the most useful key.
 */

#pragma once
#include <sys/time.h>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "cache.hpp"
#include "list.hpp"

namespace mcache {
template <class K, class V>
class lruItem : public cacheItem<K, V> {
   public:
    using cacheItem<K, V>::get_value;
    using cacheItem<K, V>::put_value;
    using cacheItem<K, V>::expire;
    using cacheItem<K, V>::is_expired;

    const K key;
    Element<lruItem<K, V>*> *element;
    struct timeval accessed_at;

    lruItem(const K _key, const V _value) : cacheItem<K, V>(_value), key(_key), element(nullptr) {}
};

template <class K, class V>
class LRU : public Cache<K, V> {
   public:
    LRU(std::size_t _max_cap) : Cache<K, V>(_max_cap) {}

    std::size_t Put(const K &key, const V &value) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            auto item = std::make_unique<lruItem<K, V>>(key, value);
            if (Size() > max_cap) {
                Evict(1);
            }
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
            if (found->second->element != nullptr) {
                _list.remove(found->second->element);
            }
            throw std::range_error("No such element in the cache");
        }

        update(found->second.get());
        return found->second->get_value();
    }

    bool Has(const K &key) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            return false;
        }
        if (found->second->is_expired()) {
            if (found->second->element != nullptr) {
                _list.remove(found->second->element);
            }
            items.erase(found);
            return false;
        }

        update(found->second.get());
        return true;
    }

    bool Remove(const K &key) noexcept override {
        auto found = items.find(key);
        if (found == items.end()) {
            return false;
        }
        if (found->second->element != nullptr) {
            _list.remove(found->second->element);
        }
        items.erase(found);
        return true;
    }

    void Evict(const int count) noexcept override {
        auto cnt = 0;
        auto *e = _list.front();
        while (e != _list.root()) {
            if (cnt >= count) {
                return;
            }
            auto *next = e->next;
            auto found = items.find(e->data->key);
            if (found != items.end()) {
                items.erase(found);
                _list.remove(e);
                cnt++;
            }
            e = next;
        }
    }

    std::size_t Size() override { return items.size(); }

    void debug() noexcept override {
        auto *e = _list.front();
        while (e != _list.root()) {
            std::cout << "[LRU] " << max_cap << "/" << Size() <<
                        ", key: " << e->data->key <<
                        ", value: " << e->data->get_value() <<
                        ", accessed_at: (" << e->data->accessed_at.tv_sec <<
                        ", " << e->data->accessed_at.tv_usec <<
                        ")" <<std::endl;
            e = e->next;
        }
    }

   protected:
    List<lruItem<K, V>*> _list;
    std::unordered_map<K, std::unique_ptr<lruItem<K, V>>> items;
    using Cache<K, V>::max_cap;

    void update(lruItem<K, V> *item) {
        gettimeofday(&item->accessed_at, nullptr);
        if (_list.size() == 0) {
            auto *element = new Element(item);
            item->element = element;
            _list.push_front(item->element);
        } else if (_list.size() > 0) {
            if (item->element != nullptr) {
                _list.move_to_front(item->element);
            } else {
                auto *element = new Element(item);
                item->element = element;
                _list.push_front(item->element);
            }
        } else {
            throw std::range_error("unreachable");
        }
    }
};

}