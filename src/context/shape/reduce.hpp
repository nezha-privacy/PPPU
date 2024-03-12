#pragma once

#include "reduce.h"

#include <optional>

#include "context/basic/basic.hpp"
#include "context/compare/compare.hpp"

#include "concatenate.hpp"

namespace pppu
{

namespace detail
{

/******************************** implemtation ********************************/

/// @brief Function template, reducing the Value to a certain extent to get its certain attributes.
/// @param fn The function to get its corresponding attribute
/// @param _in The input Value
/// @param _axis Reducing axis, for example, in matrix (dim = 2), 0 is concatenated on the row and 1 is concatenated on the column
/// @return The reduced Value
template <typename Value, typename Fn>
Value reduce(Context* ctx, Fn&& fn, Value const& _in, std::optional<int64_t> _axis)
{
    Value in;
    int64_t ndim;
    int64_t axis;

    if(_axis.has_value() == false)
    {
        in = _in.reshape({_in.numel()}); // flatten
        axis = 0;
        ndim = 1;
    }
    else
    {
        in = _in;
        ndim = _in.ndim();
        axis = _axis.value();

        if(axis < 0)    axis += ndim;
        if(axis < 0 || axis >= ndim)
            throw std::invalid_argument(fmt::format("invalid axis {}", axis));
    }

    std::vector<std::variant<core::Slice, int64_t>> mono_idx(ndim, core::Slice{});
    std::vector<std::variant<core::Slice, int64_t>> even_idx(ndim, core::Slice{});
    std::vector<std::variant<core::Slice, int64_t>> odd_idx (ndim, core::Slice{});

    mono_idx[axis] = core::Slice{.start=-1};
    even_idx[axis] = core::Slice{.stop=-1, .step=2};
    odd_idx [axis] = core::Slice{.start=1, .step=2};

    Value mono;
    bool has_mono = false;

    Value ans = in;
    while(ans.shape(axis) >= 2)
    {
        if(ans.shape(axis) % 2 == 1)
        {
            if(has_mono)
            {
                mono = std::invoke(std::forward<Fn>(fn), ctx, mono, ans.slice(mono_idx));
            }
            else
            {
                has_mono = true;
                mono = ans.slice(mono_idx);
            }
        }

        Value lhs = ans.slice(even_idx);
        Value rhs = ans.slice(odd_idx);
        ans = std::invoke(std::forward<Fn>(fn), ctx, lhs, rhs);
    }

    if(has_mono)
        ans = std::invoke(std::forward<Fn>(fn), ctx, ans, mono);

    return ans;
}

} // namespace detail

/******************************** api ********************************/

/// @brief Sum all elements in the array of input Value.
/// @param in The input Value
/// @param axis Use when the input is a matrix and need to get the sum of specific axis
/// @return The sum of all elements in the array of input Value, or the sum of specific axis
template <typename Value>
Value sum(Context* ctx, Value const& in, std::optional<int64_t> axis)
{
    return detail::reduce(ctx,
        [](Context* ctx, Value const& lhs, Value const& rhs){ return add(ctx, lhs, rhs); },
        in, axis);
}

/// @brief Get the minimum value of all elements in the array of input Value.
/// @param in The input Value
/// @param axis Use when the input is a matrix and need to get the minimum value of specific axis
/// @return The minimum value of all elements in the array of input Value, or the minimum value of specific axis
template <typename Value>
Value min(Context* ctx, Value const& in, std::optional<int64_t> axis)
{
    return detail::reduce(ctx,
        [](Context* ctx, Value const& lhs, Value const& rhs){ return min(ctx, lhs, rhs); },
        in, axis);
}

/// @brief Get the maximum value of all elements in the array of input Value.
/// @param in The input Value
/// @param axis Use when the input is a matrix and need to get the maximum value of specific axis
/// @return The maximum value of all elements in the array of input Value, or the maximum value of specific axis
template <typename Value>
Value max(Context* ctx, Value const& in, std::optional<int64_t> axis)
{
    return detail::reduce(ctx,
        [](Context* ctx, Value const& lhs, Value const& rhs){ return max(ctx, lhs, rhs); },
        in, axis);
}

/// @brief Find the maximum value and its corresponding position in the array of input Value.
/// @param in The input Value
/// @return The maximum value (single value) and its corresponding position (same shape with in) in the array of input Value
template <typename Value>
std::tuple<Value, Value> argmax(Context* ctx, Value const& in)
{
    if(in.ndim() == 0)
        throw std::invalid_argument("invalid dimension");

    if(in.ndim() > 1)
        throw std::runtime_error("argmax for multi-dim array is not implemented yet");

    if(in.numel() == 0)
        throw std::runtime_error("empty input");

    if(in.numel() == 1)
        return std::make_tuple(make_constant<Value>(ctx, 0, {1}), in);

    uint64_t tree_depth = std::bit_width( uint64_t(in.numel()) -1 );
    std::vector<Value> ges(tree_depth);

    using core::Slice;

    // compute maximum value
    Value mono;
    bool has_mono = false;
    Value max_val = in;
    for(int64_t i = tree_depth-1; i >= 0; --i)
    {
        Value lhs, rhs;
        // int64_t 
        if(int64_t n = max_val.numel(); n&1)
        {
            if(has_mono == true)
            {
                lhs = max_val.slice(std::vector<Slice>{{ .start=0, .stop=n/2+1 } });
                if(n <= 1) {
                    rhs = mono;
                }
                else {
                    rhs = concatenate(ctx, std::vector<Value>{
                        max_val.slice(std::vector<Slice>{{ .start=n/2+1, .stop=n } }),
                        mono
                    });
                }
                has_mono = false;
            }
            else
            {
                lhs  = max_val.slice(std::vector<Slice>{{.start=0,   .stop=n/2 } });
                rhs  = max_val.slice(std::vector<Slice>{{.start=n/2, .stop=n-1 } });
                mono = max_val.slice(std::vector<Slice>{{.start=n-1, .stop=n   } });
                has_mono = true;
            }
        }
        else
        {
            lhs = max_val.slice(std::vector<Slice>{{.start=0,   .stop=n/2} });
            rhs = max_val.slice(std::vector<Slice>{{.start=n/2, .stop=n  } });
        }

        Value ge = greater_equal(ctx, lhs, rhs);
        max_val = conditional(ctx, ge, rhs, lhs);

        ges[i] = ge;
    }

    Value c = concatenate(ctx, std::vector{ ges[0], logical_not(ctx, ges[0]) });
    for(int64_t i = 1; i < tree_depth; ++i)
    {
        Value d = ges[i];
        Value cc = c.slice(std::vector<Slice>{{.stop =d.numel()}});

        Value lhs = mul(ctx, cc, d);
        Value rhs = sub(ctx, cc, lhs);

        if( c.numel() > d.numel() )
        {
            Value cm = c.slice(std::vector<Slice>{{.start=d.numel()}});
            c = concatenate(ctx, std::vector{lhs, rhs, cm});
        }
        else
        {
            c = concatenate(ctx, std::vector{lhs, rhs});
        }
    }

    return std::make_tuple(c, max_val);
}


} // namespace pppu
