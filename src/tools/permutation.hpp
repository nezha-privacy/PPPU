#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "ndarray/ndarray_ref.hpp"

/// @brief Obtain the inverse permutation of a specific permutation.
/// @param perm The input permutation
/// @return The inverse permutation of a specific permutation
std::vector<int64_t> inv(std::vector<int64_t> const &perm);

/// @brief Obtain the signals of the switches of the Bernes permutation network for the input permutation, 1 for switch.
/// @note Posible optimization point: Use non-recursive algorithm. Avoid frequent slicing (variant and optional may incur heavy cost).
///       performance:
///       perm size  non-optimized  optimized(O3)
///       2^16        0.67s            0.06s
///       2^20        12s               1.2s
///       2^24                          21s
/// @param perm The input permutation
/// @return The signals for each unit in benes network, 1 for switch
core::NDArrayRef<bool> benes(std::vector<int64_t> const& perm);
