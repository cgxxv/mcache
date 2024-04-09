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
    std::vector<int> iKeys = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 222};
    std::vector<int> iVals = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1111, 2222};
    std::vector<string> sVals = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "llll", "mmmm"};

    demo<Demo<int, int>, int, int>(10, iKeys, iVals);
    std::cout << "Test Demo<int,int>            : PASSED" << std::endl;
    demo<Demo<int, string>, int, string>(10, iKeys, sVals);
    std::cout << "Test Demo<int,string>         : PASSED" << std::endl;
    demo<Demo<string, string>, string, string>(10, sVals, sVals);
    std::cout << "Test Demo<string,string>      : PASSED" << std::endl;


    demo<Simple<int, int>, int, int>(10, iKeys, iVals);
    std::cout << "Test Simple<int,int>          : PASSED" << std::endl;
    demo<Simple<int, string>, int, string>(10, iKeys, sVals);
    std::cout << "Test Simple<int,string>       : PASSED" << std::endl;
    demo<Simple<string, string>, string, string>(10, sVals, sVals);
    std::cout << "Test Simple<string,string>    : PASSED" << std::endl;

    demo<LFU<int, int>, int, int>(10, iKeys, iVals);
    std::cout << "Test LFU<int,int>             : PASSED" << std::endl;
    demo<LFU<int, string>, int, string>(10, iKeys, sVals);
    std::cout << "Test LFU<int,string>          : PASSED" << std::endl;
    demo<LFU<string, string>, string, string>(10, sVals, sVals);
    std::cout << "Test LFU<string,string>       : PASSED" << std::endl;

    demo<LRU<int, int>, int, int>(10, iKeys, iVals);
    std::cout << "Test LRU<int,int>             : PASSED" << std::endl;
    demo<LRU<int, string>, int, string>(10, iKeys, sVals);
    std::cout << "Test LRU<int,string>          : PASSED" << std::endl;
    demo<LRU<string, string>, string, string>(10, sVals, sVals);
    std::cout << "Test LRU<string,string>       : PASSED" << std::endl;

    demo<FIFO<int, int>, int, int>(10, iKeys, iVals);
    std::cout << "Test FIFO<int,int>            : PASSED" << std::endl;
    demo<FIFO<int, string>, int, string>(10, iKeys, sVals);
    std::cout << "Test FIFO<int,string>         : PASSED" << std::endl;
    demo<FIFO<string, string>, string, string>(10, sVals, sVals);
    std::cout << "Test FIFO<string,string>      : PASSED" << std::endl;

    demo<ARC<int, int>, int, int>(10, iKeys, iVals);
    std::cout << "Test ARC<int,int>             : PASSED" << std::endl;
    demo<ARC<int, string>, int, string>(10, iKeys, sVals);
    std::cout << "Test ARC<int,string>          : PASSED" << std::endl;
    demo<ARC<string, string>, string, string>(10, sVals, sVals);
    std::cout << "Test ARC<string,string>       : PASSED" << std::endl;
}

template <class T, class K, class V>
void demo(std::size_t max_cap, std::vector<K> keys, std::vector<V> vals) {
    T cc(max_cap);
    for (int i = 0; i < max_cap; i++) {
        int ret = cc.Put(keys[i], vals[i]);
        assert(ret == 1);
    }

    /*
      front->0->1->2->3->4->5->6->7->8->9->back->
    <-front<-0<-1<-2<-3<-4<-5<-6<-7<-8<-9<-back
    */

    for (int i = 0; i < max_cap; i++) {
        auto v = cc.Get(keys[i]);
        assert(v == vals[i]);
    }

    // Shuffle for LFU shuffle
    for (int i = 0; i < 10e3; i++) {
        int r = rand(0, 9);
        auto k = keys[r];
        auto v = cc.Get(k);
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

    assert(cc.Put(keys[10], vals[10]) == 1);
    assert(cc.Size() == 10);

    assert(cc.Get(keys[10]) == vals[10]);

    assert(cc.Evict(1));
    assert(cc.Size() == 9);

    assert(1 == cc.Put(keys[11], vals[11]));
    assert(cc.Remove(keys[11]));
    assert(!cc.Has(keys[11]));

    assert(1 == cc.Put(keys[12], vals[12]));
    assert(vals[12] == cc.Get(keys[12]));
    assert(10 == cc.Size());

#ifdef DEBUG_MODE
    cc.debug();
    std::cout << "========== final data ==========" << std::endl;
#endif
}
