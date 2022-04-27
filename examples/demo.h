#pragma once
#include <random>

int rand() {
    std::random_device crypto_random_generator;
    std::uniform_int_distribution<int> int_distribution(0, 9);

    return int_distribution(crypto_random_generator);
}

template<class T>
void demo(std::size_t max_cap);