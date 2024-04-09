/**
 * Copyright (c) 2022
 * FIFO has high performance, and it is a absolute O(1) time complexity
 */

#pragma once
#include <sys/time.h>

#include <iostream>

#include "cache.hpp"
#include "lfu.hpp"

namespace mcache {
template <class K, class V>
class FIFO : public LFU<K, V> {
   public:
    FIFO(std::size_t _max_cap) : LFU<K, V>(_max_cap) {}

    using LFU<K, V>::Put;
    using LFU<K, V>::Get;
    using LFU<K, V>::Has;
    using LFU<K, V>::Remove;
    using LFU<K, V>::Size;

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
                items.erase(found);
                remove(e);
                cnt++;
            }
            e = next;
        }

        return cnt == count;
    }

    void debug() noexcept override {
        auto *e = _list.front();
        while (e->data != nullptr) {
            std::cout << "[" << CACHE_FIFO << "] " << this->max_cap << "/"
                      << Size() << ", key: " << e->data->key
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
        if (item->element == nullptr) {
            auto *element = new Element(item);
            item->element = element;
            _list.push_back(item->element);
        }
    }
};

}  // namespace mcache
