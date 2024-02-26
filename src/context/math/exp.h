#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Get the x power of the base of natural logarithms.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp(x)
template <typename Value>
Value exp(Context* ctx, Value const& x);

/// @brief Get the x power of 2.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp2(x)
template <typename Value>
Value exp2(Context* ctx, Value const& x);

} // namespace pppu
