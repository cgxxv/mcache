/**
 * Copyright (c) 2022
 * FIFO has high performance, and it is a absolute O(1) time complexity
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
struct fifoItem {
    const K key;
    const V value;
    struct timeval expire;
    std::size_t _ttl;
    std::size_t accessed_at;
    Element<std::shared_ptr<fifoItem<K, V>>> *element;

    //TODO: ttl should passed by dynamic options
    fifoItem(const K _key, const V _value) : key(_key), value(_value), _ttl(3600), accessed_at(0) {
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
class FIFO : public Cacher<K, V> {
   public:
    FIFO(std::size_t _max_cap) : max_cap(_max_cap) {}

    std::size_t Set(const K &key, const V &value) noexcept override {
        auto item = std::make_shared<fifoItem<K, V>>(key, value);
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
                        ", accessed_at: " << e->data->accessed_at << std::endl;
            e = e->next;
        }
    }

    std::size_t Size() override {
        return items.size();
    }

   private:
    List<std::shared_ptr<fifoItem<K, V>>> _list;
    std::unordered_map<K, std::shared_ptr<fifoItem<K, V>>> items;
    std::size_t max_cap;

    void update(std::shared_ptr<fifoItem<K, V>> item) {
        struct timeval now;
        gettimeofday(&now, nullptr);
        item->accessed_at = now.tv_sec*10e6+now.tv_usec;

        if (item->element == nullptr) {
            auto *element = new Element(item);
            item->element = element;
            _list.push_back(item->element);
        }

        //TODO: Is this nessary?
        // if (items.size() != _list.size()) {
        //     throw std::range_error("unreachable");
        // }
    }
};

}