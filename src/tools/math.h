#pragma once

#include <cstdlib>

/// @brief Division (rounding up).
/// @param n The dividend
/// @param d The divider
/// @return The quotient
inline constexpr size_t ceildiv(size_t n, size_t d) {
    return (n+d-1) / d;
}

/// @brief Determine whether the input is a power of 2.
/// @param n The input value
/// @return Whether the input is a power of 2, true for yes
inline constexpr bool is_pow2(size_t n) {
    return (n != 0) && ((n & (n-1)) == 0);
}
