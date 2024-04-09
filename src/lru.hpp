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
#include <stdexcept>

#include "cache.hpp"
#include "lfu.hpp"

namespace mcache {
template <class K, class V>
class LRU : public LFU<K, V> {
   public:
    LRU(std::size_t _max_cap) : LFU<K, V>(_max_cap) {}

    using LFU<K, V>::Put;
    using LFU<K, V>::Get;
    using LFU<K, V>::Has;
    using LFU<K, V>::Remove;
    using LFU<K, V>::Size;

    bool Evict(const int count) noexcept override {
        if (Size() < max_cap) return false;

        auto cnt = 0;
        auto *e = _list.back();
        while (e != _list.root()) {
            if (cnt >= count) {
                break;
            }
            auto *prev = e->prev;
            auto found = items.find(e->data->key);
            if (found != items.end()) {
                items.erase(found);
                remove(e);
                cnt++;
            }
            e = prev;
        }

        return cnt == count;
    }

    void debug() noexcept override {
        auto *e = _list.front();
        while (e != _list.root()) {
            std::cout << "[" << CACHE_LRU << "] " << max_cap << "/" << Size()
                      << ", key: " << e->data->key
                      << ", value: " << e->data->get_value()
                      << ", accessed_at: (" << e->data->accessed_at->tv_sec
                      << ", " << e->data->accessed_at->tv_usec << ")"
                      << std::endl;
            e = e->next;
        }
    }

   protected:
    using LFU<K, V>::_list;
    using LFU<K, V>::items;
    using LFU<K, V>::max_cap;

    using LFU<K, V>::remove;

    void update(lfuItem<K, V> *item) override {
        if (item->accessed_at == nullptr) {
            item->accessed_at = new timeval();
        }
        gettimeofday(item->accessed_at, nullptr);
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

}  // namespace mcache
