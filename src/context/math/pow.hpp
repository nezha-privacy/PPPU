#pragma once

#include <bit>

#include "div.hpp"

#include "pow.h"

namespace pppu
{

/// @brief Get the y-th power of x Value.
/// @param ctx The calculation settings
/// @param x The input Value object to be powered
/// @param y The power number
/// @return The result, x ^ y
template <typename Value>
Value pow(Context* ctx, Value const& x, int64_t y)
{
    if(y < 0)        return pow(ctx, reciprocal(ctx, x), -y);
    else if(y == 0)  return make_constant<Value>(ctx, 1, x.shape());
    else if(y == 1)  return x;
    else if(y == 2)  return square(ctx, x);

    Value ki_1 = make_constant<Value>(ctx, 1, x.shape());

    int cntnz = sizeof(y)*8 - std::countl_zero((unsigned long long)y);

    auto bit = [](auto y, std::size_t i) { return 1&(y >> i); };

    Value ans = bit(y, 0) ? x : ki_1;
    Value x_pow = x;
    for(int i = 1; i < cntnz; ++i) {
        x_pow = square(ctx, x_pow);
        if(bit(y, i) == 1)
            ans = mul(ctx, ans, x_pow);
    }
    return ans;

}

}  // namespace pppu
