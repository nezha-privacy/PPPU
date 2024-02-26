#pragma once

#include "context/context.hpp"
#include "context/basic/basic.hpp"
#include "context/compare/compare.hpp"

namespace pppu
{

/// @brief Peforming odd-even merge sort on Value arr.
/// @param ctx The calculation settings
/// @param arr The input Value to be sorted
/// @return The Value after sorting
template <typename Value>
Value sort(Context* ctx, Value const& arr);

} // namespace PPPU
