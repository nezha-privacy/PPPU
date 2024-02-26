#include "random_generator.h"

/// @brief A function used to generate random numbers.
/// @return A random number
std::uint_fast32_t RandomGenerator::get_random(){
    return e();
}
