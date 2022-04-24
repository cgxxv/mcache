#include "simple.hpp"

#include "demo.hpp"
#include "fifo.hpp"
#include "lfu.hpp"
#include "lru.hpp"
#include "mcache.h"

using namespace mcache;

int main() {
    for (int i = 0; i < 100; i++) {
        demo<Demo<int, int>>(100);
        // demo<Simple<int, int>>(100);
        // demo<LFU<int, int>>(100);
        // demo<LRU<int, int>>(100);
        // demo<FIFO<int, int>>(100);
    }
}
