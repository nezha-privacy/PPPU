#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Get the sigmoid function of the input Value.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated
/// @return The result, sig(x)
template <typename Value>
Value sigmoid(Context* ctx, Value const& x);

} // namespace pppu
