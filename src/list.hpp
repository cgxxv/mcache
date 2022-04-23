#pragma once
#include <cstdlib>

namespace mcache {
template <class T>
class Element {
   public:
    Element() = default;
    Element(T _data) : data(_data) {}
    Element *next;
    Element *prev;
    T data;
};

template <class T>
class List {
   public:
    List() noexcept : _size(0) {
        root = new Element<T>();
        root->next = root;
        root->prev = root;
    }
    ~List() noexcept {
        auto *e = root->next;
        while (e->data != nullptr) {
            auto *next = e->next;
            delete e;
            e = next;
        }
        delete root;
    }

    std::size_t size() {
        return _size;
    }
    Element<T> *front() {
        return root->next;
    }
    Element<T> *back() {
        return root->prev;
    }
    void push_front(Element<T> *e) {
        insert(e, root);
    }
    void push_back(Element<T> *e) {
        insert(e, root->prev);
    }
    void insert_before(Element<T> *e, Element<T> *at) {
        insert(e, at->prev);
    }
    void insert_after(Element<T> *e, Element<T> *at) {
        insert(e, at);
    }
    void move_to_front(Element<T> *e) {
        move(e, root);
    }
    void move_to_back(Element<T> *e) {
        move(e, root->prev);
    }
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
    Element<T> *root;
    std::size_t _size;
    void insert(Element<T> *e, Element<T> *at) {
        //处理当前节点的相邻节点
        e->prev = at;
        e->next = at->next;

        //处理当前节点相邻节点的前后节点
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
