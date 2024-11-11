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
template <typename Value1,typename Value2>
void sort(Context* ctx, Value1  &arr1,Value2 &arr2);
} // namespace PPPU
