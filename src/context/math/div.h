#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Do division operation a / b.
/// @param ctx The calculation settings
/// @param a The input dividend Value object
/// @param b The input divisor Value object, cannot be 0
/// @return The result a/b
template <typename Value>
Value div(Context* ctx, Value const& a, Value const& b);

/// @brief Get the reciprocal of input Value.
/// @param ctx The calculation settings
/// @param in The input Value object, cannot be 0
/// @return The reciprocal of input Value
template <typename Value>
Value reciprocal(Context* ctx, Value const& in);

} // namespace pppu
