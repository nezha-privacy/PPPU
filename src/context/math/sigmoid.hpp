#pragma once

#include "sigmoid.h"

namespace pppu
{

/******************************** euler ********************************/

namespace detail
{

/// @brief Implementation of getting the sigmoid function of the input Value in mode euler.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated
/// @return The result, sig(x)
template <typename Value>
Value sigmoid_euler(Context* ctx, Value const& x)
{
    int64_t n_iters = ctx->config()->fxp_sigmoid_iters;

    Value kf_0_5 = make_constant<Value>(ctx, 0.5, x.shape());
    Value kf_1_0 = make_constant<Value>(ctx, 1.0, x.shape());
    Value kf_1_n = make_constant<Value>(ctx, 1.0/n_iters, x.shape());

    Value step = mul(ctx, x, kf_1_n);
    Value y = kf_0_5;

    for(int i = 0; i < n_iters; ++i) {
        Value t = mul(ctx, step, sub(ctx, kf_1_0, y));
        y = mul(ctx, y, add(ctx, kf_1_0, t));
    }

    return y;
}

} // namespace detail

/******************************** api ********************************/

/// @brief Get the sigmoid function of the input Value.
/// @param ctx The calculation settings
/// @param x The input Value object to be calculated
/// @return The result, sig(x)
template <typename Value>
Value sigmoid(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_sigmoid_mode)
    {
    case Config::FXP_SIGMOID_EULER:  return detail::sigmoid_euler(ctx, x);    
    default: throw std::runtime_error("unkonwn sigmoid mode");
    }
}

} // namespace pppu
