#pragma once
#include <cstdlib>

namespace mcache {
template <class T>
class Element {
   public:
    Element() = default;
    Element(T _data) : data(_data), next(nullptr), prev(nullptr) {}
    T data;
    Element *next;
    Element *prev;
};

template <class T>
class List {
   public:
    List() noexcept : _size(0) {
        _root = new Element<T>();
        _root->next = _root;
        _root->prev = _root;
    }
    ~List() noexcept {
        auto *e = _root->next;
        while (e != _root) {
            auto *next = e->next;
            remove(e);
            e = next;
        }
        delete _root;
    }

    std::size_t size() { return _size; }
    auto *root() { return _root; }
    Element<T> *front() { return _root->next; }
    Element<T> *back() { return _root->prev; }
    void push_front(Element<T> *e) { insert(e, _root); }
    void push_back(Element<T> *e) { insert(e, _root->prev); }
    void insert_before(Element<T> *e, Element<T> *at) { insert(e, at->prev); }
    void insert_after(Element<T> *e, Element<T> *at) { insert(e, at); }
    void move_to_front(Element<T> *e) { move(e, _root); }
    void move_to_back(Element<T> *e) { move(e, _root->prev); }
    void move_before(Element<T> *e, Element<T> *mark) {
        if (e == mark) {
            return;
        }
        move(e, mark->prev);
    }
    void move_after(Element<T> *e, Element<T> *mark) {
        if (e == mark) {
            return;
        }
        move(e, mark);
    }
    void remove(Element<T> *e) {
        e->prev->next = e->next;
        e->next->prev = e->prev;
        e->prev = nullptr;
        e->next = nullptr;
        if (_size > 0) {
            _size--;
        }
        delete e;
    }

   private:
    Element<T> *_root;
    std::size_t _size;
    void insert(Element<T> *e, Element<T> *at) {
        e->prev = at;
        e->next = at->next;

        e->prev->next = e;
        e->next->prev = e;
        _size++;
    }
    void move(Element<T> *e, Element<T> *at) {
        if (e == at) {
            return;
        }
        e->prev->next = e->next;
        e->next->prev = e->prev;

        e->prev = at;
        e->next = at->next;
        e->prev->next = e;
        e->next->prev = e;
    }
};

}  // namespace mcache
