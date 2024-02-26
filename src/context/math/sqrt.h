#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Get the square root of the input Value.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated
/// @return The result, sqrt(x)
template <typename Value>
Value sqrt(Context* ctx, Value const& x);

} // namespace pppu
