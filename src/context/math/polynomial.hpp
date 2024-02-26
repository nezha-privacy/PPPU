#pragma once

#include "polynomial.h"

namespace pppu
{

/// @brief Given x, get the value of a specific polynomial.
/// @param ctx The calculation settings
/// @param coef The polynomial coefficients
/// @param x The unknown number
/// @return The result, coef[0] + coef[1] * x + coef[2] * x^2 + ...
template <typename Value>
Value polynomial(Context* ctx, std::span<const Value> coef, Value const& x)
{
    if(coef.size() == 0)
        return make_constant<Value>(ctx, 0, x.shape());
    if(coef.size() == 1)
        return coef[0];

    // Value ans = coef[0];
    // Value x_pow = x;
    // for(std::size_t i = 1; i < coef.size(); ++i) {
    //     ans = add(ctx, ans, mul(ctx, coef[i], x_pow));
    //     if(i+1 < coef.size())
    //         x_pow = mul(ctx, x_pow, x);
    // }

    // Horner's method
    Value ans = coef.back();
    for(int64_t i = coef.size()-2; i >= 0; --i)
    {
        ans = add(ctx, coef[i], mul(ctx, x, ans));
    }

    return ans;
}

} // namespace pppu
