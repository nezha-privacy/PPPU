#pragma once

#include "context/context.hpp"

#include <concepts>
#include <type_traits>

namespace pppu
{

/// @brief Record the fracbits of input Value x.
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @return The result, x.fracbits() in [0, 1)
template <typename Value>
Value fraction(Context* ctx, Value const& x);

/// @brief Round a number to an integer (round down).
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @param keep_fracbits Whether to preserve fracbits after converting to integers
/// @return The result, floor(x)
///         x_floor.fracbits() == x.fracbits(),  if keep_fracbits == true
///         x_floor.fracbits() == 0,             otherwise
template <typename Value>
Value floor(Context* ctx, Value const& x, bool keep_fracbits = true);

/// @brief Round a number to an integer (round up).
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @param keep_fracbits Whether to preserve fracbits after converting to integers
/// @return The result, ceil(x)
template <typename Value>
Value ceil(Context* ctx, Value const& x, bool keep_fracbits = true);

/// @brief Round a number to an integer (half adjust).
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @param keep_fracbits Whether to preserve fracbits after converting to integers
/// @return The result, [x]
template <typename Value>
Value round(Context* ctx, Value const& x, bool keep_fracbits = true);

/// @brief Perform modulo operation on Value x.
/// @param ctx The calculation settings
/// @param x The input Value object to be moded
/// @param modules The modulus number, needs to be a integer or floating point number
/// @return The result, x mod m
template <typename Value, typename T>
requires ( std::integral<T> || std::floating_point<T> )
Value mod(Context* ctx, Value const& x, T modulus);

} // namespace pppu
