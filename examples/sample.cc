#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>

#include "simple.hpp"
#include "demo.hpp"
#include "fifo.hpp"
#include "lfu.hpp"
#include "lru.hpp"
#include "arc.hpp"
#include "demo.h"

using namespace mcache;

int main() {
    for (int i = 0; i < 100; i++) {
        demo<Demo<int, int>>(10);
        demo<Simple<int, int>>(10);
        demo<LFU<int, int>>(10);
        demo<LRU<int, int>>(10);
        demo<FIFO<int, int>>(10);
        demo<ARC<int, int>>(10);
    }
}

template<class T>
void demo(std::size_t max_cap) {
    T cc(max_cap);
    const int count = 10;
    std::vector<int> keys = {0,1,2,3,4,5,6,7,8,9};
    std::vector<int> vals = {10,11,12,13,14,15,16,17,18,19};
    for (int i = 0; i <count;i++) {
        int ret = cc.Put(keys[i], vals[i]);
        assert(ret == 1);
    }

    /*
      front->0->1->2->3->4->5->6->7->8->9->back->
    <-front<-0<-1<-2<-3<-4<-5<-6<-7<-8<-9<-back
    */

    for (int i = 0; i <count;i++) {
        const int v = cc.Get(keys[i]);
        assert(v == vals[i]);
    }

    for (int i = 0; i < 10e3; i++)
    {
        int r = rand();
        // std::cout << r << std::endl;
        const int v = cc.Get(r);
        assert(v == vals[r]);
    }

    cc.debug();
    std::cout << std::endl;

    for (int i = 0; i <count;i++) {
        const int v = cc.Get(keys[i]);
        assert(v == vals[i]);
        assert(cc.Has(keys[i]));

        if (i < 5) {
            bool ok = cc.Has(5);
            assert(ok);
        } else if (i > 5) {
            bool ok = cc.Has(5);
            assert(!ok);
        } else {
            bool ok = cc.Remove(keys[i]);
            assert(ok);

            assert(vals[i] == 10+i);
        }
    }

    cc.Evict(1);

    cc.debug();
    std::cout << std::endl;

    assert(cc.Size() == 9);

    assert(cc.Put(10, 20) == 1);
    assert(cc.Size() == 10);

    assert(cc.Get(10) == 20);

    cc.Evict(1);
    assert(cc.Size() == 9);

    //assert(!cc.Has(5));
    //assert(vals[5] == 10+keys[5]);
    //assert(keys[5] == 5);

    // assert(vals[0] == cc.Get(0));
    // assert(vals[0] == 10);
    // int v = 10000;
    // vals[0] = v;
    // assert(cc.Get(0) == 10);
}