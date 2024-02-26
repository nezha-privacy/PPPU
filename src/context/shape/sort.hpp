#pragma once

#include <stdexcept>

#include "sort.h"
#include "tools.h"

namespace pppu
{

/******************************** implemtation ********************************/

namespace detail
{

/// @brief Implementation of peforming odd-even merge sort on Value arr.
/// @param ctx The calculation settings
/// @param arr The input Value to be sorted
/// @return The Value after sorting
template <typename Value>
Value odd_even_merge_sort(Context* ctx, Value const& arr)
{
    if(arr.ndim() > 1)
        throw std::invalid_argument("not implemented");

    int64_t numel = arr.numel();
    for(auto stage: OddEvenSortStages(numel)) {
        auto [idx1, idx2] = OddEvenSortSequence(stage);

        Value x = arr.permute(idx1);
        Value y = arr.permute(idx2);

        Value gt = greater(ctx, x, y);

        Value xx = conditional(ctx, gt, x, y);
        Value yy = conditional(ctx, gt, y, x);

        arr = arr.substitute(idx1, xx).substitute(idx2, yy);        
    }

    return arr;
}

} // namespace detail

/******************************** api ********************************/

/// @brief Peforming odd-even merge sort on Value arr.
/// @param ctx The calculation settings
/// @param arr The input Value to be sorted
/// @return the Value after sorting
template <typename Value>
Value sort(Context* ctx, Value const& arr)
{
    return detail::odd_even_merge_sort(ctx, arr);
}

} // namespace pppu
