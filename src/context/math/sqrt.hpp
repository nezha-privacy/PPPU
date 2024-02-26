#pragma once

#include "sqrt.h"

#include <vector>

namespace pppu
{

/******************************** goldschmidt ********************************/

namespace detail
{

/// @brief Calculate initial guess using the least squares method.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated, in [0.5, 1)
/// @return The initial guess
template <typename Value>
Value rsqrt_initial_guess(Context* ctx, Value const& x)
{
    // // 5.08e-3
    // auto poly2 = std::vector<Value>{
    //     make_constant<Value>(ctx,  2.22552065, x.shape()),
    //     make_constant<Value>(ctx, -2.04279349, x.shape()),
    //     make_constant<Value>(ctx,  0.82004445, x.shape())
    // };

    // 1.88e-05
    auto poly5 = std::vector<Value>{
        make_constant<Value>(ctx, 3.21574848  , x.shape()),
        make_constant<Value>(ctx, -7.49435359 , x.shape()),
        make_constant<Value>(ctx, 12.42407252 , x.shape()),
        make_constant<Value>(ctx, -12.11498993, x.shape()),
        make_constant<Value>(ctx, 6.35780354  , x.shape()),
        make_constant<Value>(ctx, -1.38829093 , x.shape())
    };

    auto poly = poly5;

    return polynomial(ctx, std::span<const Value>(poly), x);
}

/// @brief Get the square root of the input Value in norm form.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated
/// @return The result, sqrt(x_norm)
template <typename Value>
Value sqrt_goldschmidt_core(Context* ctx, Value const& x)
{
    Value ki_2   = make_constant<Value>(ctx, 2,   x.shape());
    Value kf_0_5 = make_constant<Value>(ctx, 0.5, x.shape(), 1);
    Value kf_1_5 = make_constant<Value>(ctx, 1.5, x.shape(), 1);

    Value y0 = rsqrt_initial_guess(ctx, x);   // 1/sqrt(x)

    Value g = mul(ctx, y0, x);          // sqrt(x)
    Value h = mul(ctx, y0, kf_0_5);     // 1 / (2*sqrt(x))

    int n_iters = 2;
    bool use_newton = false;

    // goldschmidt iteration
    for(int i = 0; i < n_iters; ++i)
    {
        Value t = sub(ctx, kf_1_5, mul(ctx, g, h));
        h = mul(ctx, h, t);
        g = mul(ctx, g, t);
    }

    // newton iteration
    // to eliminate error accumulated by goldschmidt
    if( use_newton )
    {
        g = mul(ctx, ki_2, mul(ctx, x, h));
        Value t = sub(ctx, kf_1_5, mul(ctx, g, h));
        g = mul(ctx, g, t);
    }

    return g;
}

/// @brief Implementation of getting the square root of the input Value in mode goldschmidt.
/// @param ctx The calculation settings
/// @param _in The input Value object to be calculated
/// @return The result, sqrt(x)
template <typename Value>
Value sqrt_goldschmidt(Context* ctx, Value const& _in)
{
    // adjust input's fracbits to ctx->fxp_fracbits()
    // to properly deal with integers
    int fracbits = ctx->fxp_fracbits();
    assert( _in.fracbits() <= fracbits );
    Value x = r_lshift(ctx, _in, fracbits - _in.fracbits()).set_fracbits( fracbits );

    std::vector<Value> x_h1bitdec = h1bitdec(ctx, x, 2*x.fracbits());
    std::vector<Value> x_factor_dec ( x_h1bitdec.rbegin(), x_h1bitdec.rend() );
    Value x_factor = bitcomp(ctx, std::span<const Value>(x_factor_dec), x.fracbits());

    // x_norm in [0.5, 1)
    Value x_norm = mul(ctx, x, x_factor);
    Value x_norm_sqrt = detail::sqrt_goldschmidt_core(ctx, x_norm);

    Value ki_0 = make_constant<Value>(ctx, 0, x.shape());
    Value h_sqrt = ki_0;

    for(int64_t i = 0; i < x_h1bitdec.size(); ++i) {
        Value t = make_constant<Value>(ctx, std::pow(2, 0.5 * (1+i-x.fracbits())), x.shape());
        h_sqrt = add(ctx, h_sqrt, mul(ctx, x_h1bitdec[i], t));
    }

    return mul(ctx, x_norm_sqrt, h_sqrt);
}

} // namespace detail

/******************************** api ********************************/

/// @brief Get the square root of the input Value.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated
/// @return The result, sqrt(x)
template <typename Value>
Value sqrt(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_sqrt_mode)
    {
    case Config::FXP_SQRT_GOLDSCHMIDT:
        return detail::sqrt_goldschmidt(ctx, x);
    default:
        throw std::invalid_argument("unknown sqrt mode");
    }
}

} // namespace pppu
