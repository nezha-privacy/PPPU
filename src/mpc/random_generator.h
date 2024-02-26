#pragma once

#include <random>
#include <ctime>

/// @brief A class used to generate random numbers.
class RandomGenerator{
private:
    std::default_random_engine e;
public:
    /// @brief The constructor.
    /// @param seed The random seed
    RandomGenerator(long seed): e(seed){};
    std::uint_fast32_t get_random();
};
