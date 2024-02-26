#pragma once

#include <span>

#include "context/context.hpp"

namespace pppu
{

/// @brief Given x, get the value of a specific polynomial.
/// @param ctx The calculation settings
/// @param coef The polynomial coefficients
/// @param x The unknown number
/// @return The result, coef[0] + coef[1] * x + coef[2] * x^2 + ...
template <typename Value>
Value polynomial(Context* ctx, std::span<const Value> coef, Value const& x);

} // namespace pppu
