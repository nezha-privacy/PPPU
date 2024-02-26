#pragma once

#include <optional>
#include <cstdlib>

#include "context/context.hpp"

namespace pppu
{

/// @brief Sum all elements in the array of input Value.
/// @param in The input Value
/// @param axis Use when the input is a matrix and need to get the sum of specific axis
/// @return The sum of all elements in the array of input Value, or the sum of specific axis
template <typename Value>
Value sum(Context* ctx, Value const& in, std::optional<int64_t> axis = std::optional<int64_t>{});

/// @brief Get the minimum value of all elements in the array of input Value.
/// @param in The input Value
/// @param axis Use when the input is a matrix and need to get the minimum value of specific axis
/// @return The minimum value of all elements in the array of input Value, or the minimum value of specific axis
template <typename Value>
Value min(Context* ctx, Value const& in, std::optional<int64_t> axis = std::optional<int64_t>{});

/// @brief Get the maximum value of all elements in the array of input Value.
/// @param in The input Value
/// @param axis Use when the input is a matrix and need to get the maximum value of specific axis
/// @return The maximum value of all elements in the array of input Value, or the maximum value of specific axis
template <typename Value>
Value max(Context* ctx, Value const& in, std::optional<int64_t> axis = std::optional<int64_t>{});

// TODO not implement yet 
template <typename Value>
std::tuple<Value, Value> argmin(Context* ctx, Value const& in);

/// @brief Find the maximum value and its corresponding position in the array of input Value.
/// @param in The input Value
/// @return The maximum value (single value) and its corresponding position (same shape with in) in the array of input Value
template <typename Value>
std::tuple<Value, Value> argmax(Context* ctx, Value const& in);

} // namespace pppu
