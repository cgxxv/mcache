/**
 * Copyright (c) 2022
 * Arc is the blance for lru and lfu.
 */

#pragma once
#include <sys/time.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <thread>

#include "cache.hpp"
#include "list.hpp"
#include "lfu.hpp"

namespace mcache {
template <class K>
class arcList {
   public:
    arcList() = default;

    bool has(const K &key) {
        auto found = _keys.find(key);
        if (found != _keys.end()) return true;
        return false;
    }

    Element<K> *lookup(const K &key) {
        auto found = _keys.find(key);
        if (found != _keys.end()) {
            return found->second;
        }
        return nullptr;
    }

    void moveToFront(Element<K> *e) { _list.move_to_front(e); }

    void pushFront(const K &key) {
        auto found = _keys.find(key);
        if (found != _keys.end()) {
            _list.move_to_front(found->second);
            return;
        }

        auto *e = new Element(key);
        _list.push_front(e);
        _keys.emplace(key, e);
    }

    void remove(const K &key) {
        auto found = _keys.find(key);
        if (found == _keys.end()) {
            return;
        }
        _list.remove(found->second);
        _keys.erase(found);
    }

    void removeElement(const K &key, Element<K> *e) {
        if (e == nullptr) {
            throw std::range_error("Empty element to remove");
        }
        _list.remove(e);
        _keys.erase(key);
    }

    Element<K> *removeTail() {
        auto *e = _list.back();
        if (e != nullptr) {
            _keys.erase(e->data);
            _list.remove(e);
            return e;
        }
        return nullptr;
    }

    std::size_t len() { return _list.size(); }

    auto *front() { return _list.front(); }
    auto *root() { return _list.root(); }

   private:
    List<K> _list;
    std::unordered_map<K, Element<K> *> _keys;
};

template <class K, class V>
class ARC : public LFU<K, V> {
   public:
    ARC(std::size_t _max_cap) : LFU<K, V>(_max_cap) {}

    using LFU<K, V>::Put;
    using LFU<K, V>::Get;
    using LFU<K, V>::Has;
    using LFU<K, V>::Remove;
    using LFU<K, V>::Size;

    bool Evict(const int count) noexcept override {
        if (Size() < max_cap) return false;

        auto cnt = 0;
        while (cnt < count) {
            if (isCacheFull() && t1.len() + b1.len() == max_cap) {
                if (t1.len() >= max_cap) {
                    auto e = t1.removeTail();
                    if (e != nullptr) {
                        remove2(e->data);
                        delete e;
                        cnt++;
                    }
                } else {
                    if (b1.len() > 0) {
                        auto e = b1.removeTail();
                        if (e != nullptr) {
                            remove2(e->data);
                            delete e;
                            cnt++;
                        }
                    }
                }
            } else {
                auto total = t1.len() + t2.len() + b1.len() + b2.len();
                if (total == max_cap<<1) {
                    if (b2.len() > 0) {
                        auto e = b2.removeTail();
                        if (e != nullptr) {
                            remove2(e->data);
                            delete e;
                            cnt++;
                            continue;
                        }
                    }
                    if (b1.len() > 0) {
                        auto e = b1.removeTail();
                        if (e != nullptr) {
                            remove2(e->data);
                            delete e;
                            cnt++;
                        }
                    }
                }
            }
        }

        return cnt == count;
    }

    void debug() override {
        debug2(b1);
        debug2(t1);
        debug2(t2);
        debug2(b2);
    }

   protected:
    arcList<K> t1;
    arcList<K> t2;
    arcList<K> b1;
    arcList<K> b2;
    using LFU<K, V>::items;
    using LFU<K, V>::max_cap;

    void remove(Element<lfuItem<K, V> *> *e) override {
        t1.remove(e->data->key);
        t2.remove(e->data->key);
        b1.remove(e->data->key);
        b2.remove(e->data->key);
        delete e;
    }

    void update(lfuItem<K, V> *item) override {
        if (item->freq == nullptr) {
            item->freq = new std::size_t(0);
        }
        *item->freq += 1;

        if (item->accessed_at == nullptr) {
            item->accessed_at = new timeval();
        }
        gettimeofday(item->accessed_at, nullptr);

        //only for the first time
        if (t1.has(item->key) || t2.has(item->key)) return;

        if (item->element == nullptr) {
            item->element = new Element(item);
            t1.pushFront(item->key);
        } else {
            //update the order
            update2(item->key);
        }
    }

   private:
    void update2(const K &key) noexcept {
        auto e = b1.lookup(key);
        if (e != nullptr) {
            b1.removeElement(key, e);
            delete e;
            t1.pushFront(key);
            return;
        }

        e = t1.lookup(key);
        if (e != nullptr) {
            t1.removeElement(key, e);
            delete e;
            t2.pushFront(key);
            return;
        }

        e = t2.lookup(key);
        if (e != nullptr) {
            t2.moveToFront(e);
            return;
        }

        e = b2.lookup(key);
        if (e != nullptr) {
            b2.removeElement(key, e);
            delete e;
            t1.pushFront(key);
            if (isCacheFull() && t1.len() > 0) {
                auto ele = t1.removeTail();
                b1.pushFront(ele->data);
            }
        }
    }

    bool isCacheFull() {
        return t1.len() + t2.len() == max_cap;
    }

    void remove2(const K &key) {
        auto found = items.find(key);
        if (found != items.end()) {
            delete found->second->element;
            delete found->second->freq;
            delete found->second->accessed_at;
            items.erase(found);
        }
    }

    void debug2(arcList<K> &l) {
        auto *e = l.front();
        while (e != l.root()) {
            auto key = e->data;
            auto found = items.find(key);
            if (found == items.end()) {
                // std::char[] buf( new char[ 1024 ] );
                // std::sprintf(buf, "Unexpected error for key (%s) missing", key);
                throw std::runtime_error("Unexpected error for key (%s) missing");
            }
            auto item = found->second.get();
            std::cout << "[" << CACHE_ARC << "] " << max_cap << "/" << Size()
                      << ", key: " << key
                      << ", value: " << item->get_value()
                      << ", freq: " << *item->freq
                      << ", accessed_at: (" << item->accessed_at->tv_sec
                      << ", " << item->accessed_at->tv_usec << ")"
                      << std::endl;
            e = e->next;
        }
    }
};

}  // namespace mcache
