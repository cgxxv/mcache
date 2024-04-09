#pragma once
#include <random>

using namespace mcache;

int rand(int start, int end) {
    std::random_device crypto_random_generator;
    std::uniform_int_distribution<int> int_distribution(start, end);

    return int_distribution(crypto_random_generator);
}

template <class T, class K, class V>
void demo(std::size_t max_cap, std::vector<K> keys, std::vector<V> vals);