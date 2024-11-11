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


template <typename Value1,typename Value2>
void odd_even_merge_sort(Context* ctx, Value1& arr1,Value2& arr2)
{
    if(arr1.ndim() > 1)
        throw std::invalid_argument("not implemented");

    int64_t numel = arr1.numel();
    for(auto stage: OddEvenSortStages(numel)) {
        auto [idx1, idx2] = OddEvenSortSequence(stage);

        Value x1 = arr1.permute(idx1);
        Value y1 = arr1.permute(idx2);
        Value x2 = arr2.permute(idx1);
        Value y2 = arr2.permute(idx2);

        Value gt = greater(ctx, x1, y1);

        Value xx1 = conditional(ctx, gt, x1, y1);
        Value yy1 = conditional(ctx, gt, y1, x1);
        Value xx2 = conditional(ctx, gt, x2, y2);
        Value yy2 = conditional(ctx, gt, y2, x2);

        arr1 = arr1.substitute(idx1, xx1).substitute(idx2, yy1);
        arr2 = arr2.substitute(idx1, xx2).substitute(idx2, yy2);                
    }
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

template <typename Value1,typename Value2>
void sort(Context* ctx, Value1  &arr1,Value2  &arr2)
{
    return detail::odd_even_merge_sort(ctx, arr1,arr2);
}
} // namespace pppu
