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
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "cache.hpp"
#include "list.hpp"

namespace mcache {
template <class K, class V>
struct lfuItem {
    const K key;
    const V value;
    struct timeval expire;
    std::size_t _ttl;
    std::size_t freq;
    Element<std::shared_ptr<lfuItem<K, V>>> *element;

    //TODO: ttl should passed by dynamic options
    lfuItem(const K _key, const V _value) : key(_key), value(_value), _ttl(3600), freq(0) {
        gettimeofday(&expire, nullptr);
        expire.tv_sec += _ttl;
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
class LFU : public Cacher<K, V> {
   public:
    LFU(std::size_t _max_cap) : max_cap(_max_cap) {}

    std::size_t Set(const K &key, const V &value) noexcept override {
        auto item = std::make_shared<lfuItem<K, V>>(key, value);
        auto found = items.find(key);
        if (found == items.end()) {
            if (Size() > max_cap) {
                Evict(1);
            }
            items.emplace(key, item);
            return 1;
        }
        _list.remove(found->second->element);
        found->second->element = nullptr;
        found->second = item;
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

        update(found->second);
        return found->second->value;
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

        update(found->second);
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
        while (e->data != nullptr) {
            if (cnt >= count) {
                return;
            }
            auto *temp = e->next;
            auto found = items.find(e->data->key);
            if (found != items.end()) {
                items.erase(found);
                _list.remove(e);
                cnt++;
            }
            e = temp;
        }
    }

    void debug() noexcept {
        auto *e = _list.front();
        while (e->data != nullptr) {
            std::cout << "key: " << e->data->key <<
                        ", val: " << e->data->value <<
                        ", freq: " << e->data->freq << std::endl;
            e = e->next;
        }
    }

    std::size_t Size() override {
        return items.size();
    }

   private:
    List<std::shared_ptr<lfuItem<K, V>>> _list;
    std::unordered_map<K, std::shared_ptr<lfuItem<K, V>>> items;
    std::size_t max_cap;

    //TODO: the most worse time complexity will be O(n) for the insertion sort.
    //The insertion sort may has bad performance, will be evaluated and optimized in the future.
    void update(std::shared_ptr<lfuItem<K, V>> item) {
        item->freq += 1;
        if (_list.size() == 0) {
            auto *element = new Element(item);
            item->element = element;
            _list.push_back(item->element);
        } else if (_list.size() > 0) {
            auto found = false;
            auto *e = _list.front();
            while (e->data != nullptr) {
                if (e->data->freq >= item->freq && e->data->key != item->key) {
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

        //TODO: Is this nessary?
        // if (items.size() != _list.size()) {
        //     throw std::range_error("unreachable");
        // }
    }
};

}
