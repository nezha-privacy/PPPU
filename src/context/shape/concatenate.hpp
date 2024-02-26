#pragma once

#include "concatenate.h"

#include "context/basic/raw.hpp"
#include "context/visibility.h"

#include "ndarray/concatenate.hpp"

namespace pppu
{

/// @brief Concatenate a set of Value end-to-end.
/// @param ctx The calculation settings
/// @param values A set of Value to be concatenated
/// @param axis Concatenating axis, for example, in matrix (dim = 2), 0 is concatenated on the row and 1 is concatenated on the column
/// @return The Value after concatenating
template <typename Value>
Value concatenate(Context* ctx, std::vector<Value> values, int64_t axis)
{
    Visibility visibility = values.front().visibility();
    int64_t    fracbits   = values.front().fracbits();

    // check whether visibility are the same
    // if fracbits are not the same, get max
    for(auto& x: values)
    {
        if(x.visibility() != visibility)
            throw std::invalid_argument("inconsistent visibility");
        if(x.fracbits() != fracbits)
            fracbits = std::max(x.fracbits(), fracbits);
    }

    Value ans;

    if(visibility.is_share())
    {
        std::vector<typename Value::ShareType> input_data;
        for(auto& x: values)
        {
            input_data.emplace_back(
                r_lshift(ctx, x, fracbits-x.fracbits()).data_s()
            );
        }
        auto output_data = core::concatenate(std::span(input_data), axis);
        ans.assign_s(output_data);
    }
    else if( visibility.is_public() || visibility.is_private() )
    {
        std::vector<typename Value::PlainType> input_data;
        for(auto& x: values)
        {
            input_data.emplace_back(
                r_lshift(ctx, x, fracbits-x.fracbits()).data_p()
            );
        }
        auto output_data = core::concatenate(std::span(input_data), axis);
        ans.assign_p(output_data);
    }


    ans.set_fracbits(fracbits);
    ans.set_visibility(visibility);
    return ans;
}

} // namespace pppu
