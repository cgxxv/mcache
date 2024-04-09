#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "arc.hpp"
#include "demo.h"
#include "demo.hpp"
#include "fifo.hpp"
#include "lfu.hpp"
#include "lru.hpp"
#include "simple.hpp"

using namespace mcache;

int main() {
    demo<Demo<int, int>>(10);
    std::cout << "Test Demo<int,int>   : PASSED" << std::endl;
    demo<Simple<int, int>>(10);
    std::cout << "Test Simple<int,int> : PASSED" << std::endl;
    demo<LFU<int, int>>(10);
    std::cout << "Test LFU<int,int>    : PASSED" << std::endl;
    demo<LRU<int, int>>(10);
    std::cout << "Test LRU<int,int>    : PASSED" << std::endl;
    demo<FIFO<int, int>>(10);
    std::cout << "Test FIFO<int,int>   : PASSED" << std::endl;
    demo<ARC<int, int>>(10);
    std::cout << "Test ARC<int,int>    : PASSED" << std::endl;
}

template <class T>
void demo(std::size_t max_cap) {
    T cc(max_cap);
    const int count = 10;
    std::vector<int> keys = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> vals = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    for (int i = 0; i < count; i++) {
        int ret = cc.Put(keys[i], vals[i]);
        assert(ret == 1);
    }

    /*
      front->0->1->2->3->4->5->6->7->8->9->back->
    <-front<-0<-1<-2<-3<-4<-5<-6<-7<-8<-9<-back
    */

    for (int i = 0; i < count; i++) {
        const int v = cc.Get(keys[i]);
        assert(v == vals[i]);
    }

    // Shuffle for LFU shuffle
    for (int i = 0; i < 10e3; i++) {
        int r = rand(0, 9);
        const int v = cc.Get(r);
        assert(v == vals[r]);
    }

#ifdef DEBUG_MODE
    cc.debug();
    std::cout << "========== before eviction ==========" << std::endl;
#endif

    assert(cc.Size() == 10);
    assert(cc.Evict(1));
    assert(cc.Size() == 9);

#ifdef DEBUG_MODE
    cc.debug();
    std::cout << "========== after eviction ==========" << std::endl;
#endif

    assert(cc.Put(10, 20) == 1);
    assert(cc.Size() == 10);

    assert(cc.Get(10) == 20);

    assert(cc.Evict(1));
    assert(cc.Size() == 9);

    assert(1 == cc.Put(11, 1111));
    assert(cc.Remove(11));
    assert(!cc.Has(11));

    assert(1 == cc.Put(222, 2222));
    assert(2222 == cc.Get(222));
    assert(10 == cc.Size());

#ifdef DEBUG_MODE
    cc.debug();
    std::cout << "========== final data ==========" << std::endl;
#endif
}
