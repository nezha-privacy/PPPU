#pragma once

#include <vector>
#include <span>

#include "context/context.hpp"

namespace pppu
{

/// @brief Concatenate a set of Value end-to-end.
/// @param ctx The calculation settings
/// @param values A set of Value to be concatenated
/// @param axis Concatenating axis, for example, in matrix (dim = 2), 0 is concatenated on the row and 1 is concatenated on the column
/// @return The Value after concatenating
template <typename Value>
Value concatenate(Context* ctx, std::vector<Value> values, int64_t axis = 0);

} // namespace pppu
