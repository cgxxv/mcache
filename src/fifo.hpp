/**
 * Copyright (c) 2022
 * FIFO has high performance, and it is a absolute O(1) time complexity
 */

#pragma once
#include <sys/time.h>
#include <iostream>
#include <memory>

#include "cache.hpp"
#include "lru.hpp"

namespace mcache {
template <class K, class V>
class FIFO : public LRU<K, V> {
   public:
    FIFO(std::size_t _max_cap) : LRU<K, V>(_max_cap) {}
    ~FIFO() = default;

    using LRU<K, V>::Set;
    using LRU<K, V>::Get;
    using LRU<K, V>::Has;
    using LRU<K, V>::Remove;
    using LRU<K, V>::Size;

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

    void debug() noexcept override {
        auto *e = _list.front();
        while (e->data != nullptr) {
            std::cout << "[FIFO] " << this->max_cap << "/" << Size() <<
                        ", key: " << e->data->key <<
                        ", val: " << e->data->value() <<
                        ", accessed_at: (" << e->data->accessed_at.tv_sec <<
                        ", " << e->data->accessed_at.tv_usec <<
                        ")" <<std::endl;
            e = e->next;
        }
    }

   private:
    using LRU<K, V>::_list;
    using LRU<K, V>::items;

    void update(std::shared_ptr<lruItem<K, V>> item) {
        gettimeofday(&item->accessed_at, nullptr);

        if (item->element == nullptr) {
            auto *element = new Element(item);
            item->element = element;
            _list.push_back(item->element);
        }
    }
};

}