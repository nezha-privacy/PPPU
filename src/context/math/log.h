#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Get the natural logarithm of input data.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log(x)
template <typename Value>
Value log(Context* ctx, Value const& x);

/// @brief Get the base 2 logarithm of input data.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log2(x)
template <typename Value>
Value log2(Context* ctx, Value const& x);

/// @brief Get the base 10 logarithm of input data.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log10(x)
template <typename Value>
Value log10(Context* ctx, Value const& x);

} // namespace pppu
