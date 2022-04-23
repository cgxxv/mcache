#include "demo.hpp"
#include "simple.hpp"
#include "lfu.hpp"
#include "lru.hpp"
#include "fifo.hpp"

#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <random>

using namespace std;
using namespace mcache;

int rand() {
    std::random_device crypto_random_generator;
    std::uniform_int_distribution<int> int_distribution(0, 9);

    return int_distribution(crypto_random_generator);
}

template <class T>
void demo();

int main() {
    demo<Demo<int, int>>();
    demo<Simple<int, int>>();
    demo<LFU<int, int>>();
    demo<LRU<int, int>>();
    demo<FIFO<int, int>>();
}

template <class T>
void demo() {
    const int count = 10;
    T cc(100);
    vector<int> keys = {0,1,2,3,4,5,6,7,8,9};
    vector<int> vals = {10,11,12,13,14,15,16,17,18,19};
    for (int i = 0; i <count;i++) {
        int ret = cc.Set(keys[i], vals[i]);
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
    assert(cc.Size() == 8);

    assert(!cc.Has(5));
    assert(vals[5] == 10+keys[5]);
    assert(keys[5] == 5);

    // assert(vals[0] == cc.Get(0));
    // assert(vals[0] == 10);
    // int v = 10000;
    // vals[0] = v;
    // assert(cc.Get(0) == 10);
}