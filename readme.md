# mcache

MCache is a local memory cache with some modern features, implemented LFU, LRU, FIFO, ARC, simple evication.

## examples

```bash
mkdir build
cd build
#see more details
#cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
make simple && ./simple
make cache && ./cache
```
