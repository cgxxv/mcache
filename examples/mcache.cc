#include "mcache.hpp"

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
#include "options.hpp"
#include "simple.hpp"

using namespace mcache;

int main() {
    std::vector<int> iKeys = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 222};
    std::vector<int> iVals = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1111, 2222};
    std::vector<string> sVals = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "llll", "mmmm"};

    demo<MCache<int, int, Demo>, int, int>(10, iKeys, iVals);
    std::cout << "Test MCache<int, int, Demo>           : PASSED" << std::endl;
    demo<MCache<int, string, Demo>, int, string>(10, iKeys, sVals);
    std::cout << "Test MCache<int, string, Demo>        : PASSED" << std::endl;
    demo<MCache<string, string, Demo>, string, string>(10, sVals, sVals);
    std::cout << "Test MCache<string, string, Demo>     : PASSED" << std::endl;

    demo<MCache<int, int, Simple>, int, int>(10, iKeys, iVals);
    std::cout << "Test MCache<int, int, Simple>         : PASSED" << std::endl;
    demo<MCache<int, string, Simple>, int, string>(10, iKeys, sVals);
    std::cout << "Test MCache<int, string, Simple>      : PASSED" << std::endl;
    demo<MCache<string, string, Simple>, string, string>(10, sVals, sVals);
    std::cout << "Test MCache<string, string, Simple>   : PASSED" << std::endl;

    demo<MCache<int, int, LFU>, int, int>(10, iKeys, iVals);
    std::cout << "Test MCache<int, int, LFU>            : PASSED" << std::endl;
    demo<MCache<int, string, LFU>, int, string>(10, iKeys, sVals);
    std::cout << "Test MCache<int, string, LFU>         : PASSED" << std::endl;
    demo<MCache<string, string, LFU>, string, string>(10, sVals, sVals);
    std::cout << "Test MCache<string, string, LFU>      : PASSED" << std::endl;

    demo<MCache<int, int, LRU>, int, int>(10, iKeys, iVals);
    std::cout << "Test MCache<int, int, LRU>            : PASSED" << std::endl;
    demo<MCache<int, string, LRU>, int, string>(10, iKeys, sVals);
    std::cout << "Test MCache<int, string, LRU>         : PASSED" << std::endl;
    demo<MCache<string, string, LRU>, string, string>(10, sVals, sVals);
    std::cout << "Test MCache<string, string, LRU>      : PASSED" << std::endl;

    demo<MCache<int, int, FIFO>, int, int>(10, iKeys, iVals);
    std::cout << "Test MCache<int, int, FIFO>           : PASSED" << std::endl;
    demo<MCache<int, string, FIFO>, int, string>(10, iKeys, sVals);
    std::cout << "Test MCache<int, string, FIFO>        : PASSED" << std::endl;
    demo<MCache<string, string, FIFO>, string, string>(10, sVals, sVals);
    std::cout << "Test MCache<string, string, FIFO>     : PASSED" << std::endl;

    demo<MCache<int, int, ARC>, int, int>(10, iKeys, iVals);
    std::cout << "Test MCache<int, int, ARC>            : PASSED" << std::endl;
    demo<MCache<int, string, ARC>, int, string>(10, iKeys, sVals);
    std::cout << "Test MCache<int, string, ARC>         : PASSED" << std::endl;
    demo<MCache<string, string, ARC>, string, string>(10, sVals, sVals);
    std::cout << "Test MCache<string, string, ARC>      : PASSED" << std::endl;
}

template <class T, class K, class V>
void demo(std::size_t max_cap, std::vector<K> keys, std::vector<V> vals) {
    option opt = WithName("demo");

    T cc(max_cap);
    for (int i = 0; i < max_cap; i++) {
        int ret = cc.Put(keys[i], vals[i], opt);
        assert(ret == 1);
    }

    /*
      front->0->1->2->3->4->5->6->7->8->9->back->
    <-front<-0<-1<-2<-3<-4<-5<-6<-7<-8<-9<-back
    */

    for (int i = 0; i < max_cap; i++) {
        auto v = cc.Get(keys[i], opt);
        assert(v == vals[i]);
    }

    // Shuffle for LFU shuffle
    for (int i = 0; i < 10e3; i++) {
        int r = rand(0, 9);
        auto k = keys[r];
        auto v = cc.Get(k, opt);
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

    assert(cc.Put(keys[10], vals[10], opt) == 1);
    assert(cc.Size() == 10);

    assert(cc.Get(keys[10], opt) == vals[10]);

    assert(cc.Evict(1));
    assert(cc.Size() == 9);

    assert(1 == cc.Put(keys[11], vals[11], opt));
    assert(cc.Remove(keys[11]));
    assert(!cc.Has(keys[11]));

    assert(1 == cc.Put(keys[12], vals[12], opt));
    assert(vals[12] == cc.Get(keys[12], opt));
    assert(10 == cc.Size());

#ifdef DEBUG_MODE
    cc.debug();
    std::cout << "========== final data ==========" << std::endl;
#endif
}
