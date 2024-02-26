#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Get the y-th power of x Value.
/// @param ctx The calculation settings
/// @param x The input Value object to be powered
/// @param y The power number
/// @return The result, x ^ y
template <typename Value>
Value pow(Context* ctx, Value const& lhs, int64_t rhs);

} // namespace pppu