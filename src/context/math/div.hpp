#pragma once

#include "div.h"

namespace pppu
{

/******************************** newton ********************************/

namespace detail
{

/// @brief Get the factor used in function reciprocal_newton_norm.
/// @param ctx The calculation settings
/// @param in The input Value object, should be positive fixed point number
/// @return The factor, such that in*factor is in [0.5, 1]
template <typename Value>
Value div_factor(Context* ctx, Value const& _in)
{
    // adjust input's fracbits to ctx->fxp_fracbits()
    // to properly deal with integers
    int fracbits = ctx->fxp_fracbits();
    assert( _in.fracbits() <= fracbits );
    Value in = r_lshift(ctx, _in, fracbits - _in.fracbits()).set_fracbits( fracbits, true );

    std::vector<Value> x_h1bitdec = h1bitdec(ctx, in, 2*fracbits);
    std::vector<Value> x_factor_dec ( x_h1bitdec.rbegin(), x_h1bitdec.rend() );

    Value x_factor = bitcomp(ctx, std::span<const Value>(x_factor_dec), fracbits);

    return x_factor;
}

/// @brief Get the reciprocal of b through multiplication approximation, 
///        and the specific accuracy depends on the parameters in ctx.
/// @param ctx The calculation settings
/// @param b The input Value object, is in [0.5, 1]
/// @return The result 1 / b
template <typename Value>
Value reciprocal_newton_norm(Context* ctx, Value const& b)
{
    int n_iters = ctx->config()->fxp_div_iters;

    Value kf_2      = make_constant<Value>(ctx, 2.0,    b.shape());
    Value kf_2_9142 = make_constant<Value>(ctx, 2.9142, b.shape());

    Value initial_guess = sub(ctx, kf_2_9142, mul(ctx, kf_2, b));

    Value x = initial_guess;
    for(int i = 0; i < n_iters; ++i) {
        x = mul(ctx, x, sub(ctx, kf_2, mul(ctx, b, x)));
    }
    return x;
}

/// @brief Implementation of division operation a / b in mode newton.
/// @param ctx The calculation settings
/// @param a The input dividend Value object
/// @param b The input divisor Value object, cannot be 0
/// @return The result a / b
template <typename Value>
Value div_newton(Context* ctx, Value const& a, Value const& b)
{
    Value b_sign   = sign(ctx, b);
    Value b_abs    = mul(ctx, b, b_sign);
    Value b_factor = div_factor(ctx, b_abs);
    Value b_norm   = mul(ctx, b_abs, b_factor);

    Value b_norm_reciprocal = reciprocal_newton_norm(ctx, b_norm);

    Value a1  = mul(ctx, a,  b_sign);
    Value a2  = mul(ctx, a1, b_factor);
    Value ans = mul(ctx, a2, b_norm_reciprocal);
    return ans;
}

} // namespace detail

/******************************** api ********************************/

/// @brief Do division operation a / b.
/// @param ctx The calculation settings
/// @param a The input dividend Value object
/// @param b The input divisor Value object, cannot be 0
/// @return The result a/b
template <typename Value>
Value div(Context* ctx, Value const& a, Value const& b)
{
    switch(ctx->config()->fxp_div_mode)
    {
        case Config::FXP_DIV_NEWTON:  return detail::div_newton(ctx, a, b);
        default: throw std::runtime_error("unkonwn division mode");
    }
}

/// @brief Get the reciprocal of input Value.
/// @param ctx The calculation settings
/// @param in The input Value object, cannot be 0
/// @return The reciprocal of input Value
template <typename Value>
Value reciprocal(Context* ctx, Value const& in)
{
    Value ki_1 = make_constant<Value>(ctx, 1, in.shape());
    return div(ctx, ki_1, in);
}

} // namespace pppu
