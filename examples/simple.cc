#include "demo.hpp"
#include "simple.hpp"
#include "lfu.hpp"
#include "lru.hpp"
#include "fifo.hpp"
#include "mcache.h"

using namespace mcache;

int main() {
    demo<Demo<int, int>>(100);
    demo<Simple<int, int>>(100);
    demo<LFU<int, int>>(100);
    demo<LRU<int, int>>(100);
    demo<FIFO<int, int>>(100);
}