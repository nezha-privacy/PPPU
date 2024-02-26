#pragma once

#include "log.h"

#include "polynomial.hpp"

#include <tuple>

namespace pppu
{

/******************************** taylor ********************************/

namespace detail
{

/// @brief Get a pair of numbers (g, h) required for logarithmic operations.
/// @param ctx The calculation settings
/// @param in The input Value to be logarithmized
/// @return (g, h) such that x = g * 2^h, where g in [0.5, 1) and h is integer
template <typename Value>
std::tuple<Value, Value> log2_dec(Context* ctx, Value const& in)
{
    // adjust input's fracbits to ctx->fxp_fracbits()
    // to properly deal with integers when computing x_factor
    assert( in.fracbits() <= ctx->fxp_fracbits() );
    Value x = r_lshift(ctx, in, ctx->fxp_fracbits() - in.fracbits()).set_fracbits( ctx->fxp_fracbits() );

    // bitfloor(x) is the 2's power in [x/2, x)
    // x_h1bitdec is the decomposition of bitfloor(x)
    // x_factor = 1 / ( 2*bitfloor(x) )
    std::vector<Value> x_h1bitdec = h1bitdec(ctx, x, 2*x.fracbits());
    std::vector<Value> x_factor_dec ( x_h1bitdec.rbegin(), x_h1bitdec.rend() );
    Value x_factor = bitcomp(ctx, std::span<const Value>(x_factor_dec), x.fracbits());

    // x = x_norm * x_factor
    // x_norm is in [0.5, 1)
    Value x_norm = mul(ctx, x, x_factor);

    auto const& shape = x.shape();

    Value ki_0 = make_constant<Value>(ctx, 0, shape);
    Value ki_1 = make_constant<Value>(ctx, 1, shape);

    // log2(bitfloor(x)) is the integer in [ log2(x)-1, log2(x) )
    Value log2_bitfloor_x = ki_0;
    for(int i = 0; i < x_h1bitdec.size(); ++i) {
        Value ki_i = make_constant<Value>(ctx, i-x.fracbits(), shape);
        log2_bitfloor_x = add(ctx, log2_bitfloor_x, mul(ctx, x_h1bitdec[i], ki_i));
    }

    // log2(x)
    // = log2(x_norm / x_factor)
    // = log2(x_norm) + log2(bitfloor(x)) + 1
    // = log2(g) + h
    Value g = x_norm;
    Value h = add(ctx, ki_1, log2_bitfloor_x);

    return { g, h };
}

/// @brief The core taylor expansion used to perform logarithmic operations.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized, in (0, 2]
/// @return ln(x) = ln(1-t) = -t - t^2/2 - t^3/3 - ...
template <typename Value>
Value log_taylor_core(Context* ctx, Value const& x)
{
    int64_t n_iters = std::max(int64_t(2), ctx->config()->fxp_log_iters);

    std::vector<Value> coef (n_iters);

    Value ki_0 = make_constant<Value>(ctx, 0, x.shape());
    Value ki_1 = make_constant<Value>(ctx, 1, x.shape());

    coef[0] = ki_0;
    coef[1] = ki_1;
    for(int64_t i = 2; i < n_iters; ++i) {
        coef[i] = make_constant<Value>(ctx, 1.0 / i, x.shape());
    }

    return neg(ctx, polynomial(ctx, std::span<const Value>(coef), sub(ctx, ki_1, x)) );
}

/// @brief Implementation of getting the natural logarithm of input data in mode taylor.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log(x)
template<typename Value>
Value log_taylor(Context* ctx, Value const& x)
{
    auto [g, h] = log2_dec(ctx, x);

    Value kf_ln_2 = make_constant<Value>(ctx, std::numbers::ln2_v<double>, x.shape());

    Value ln_g = log_taylor_core(ctx, g);
    Value ln_x = add(ctx, ln_g, mul(ctx, kf_ln_2, h));

    return ln_x;
}

/// @brief Implementation of getting the base 2 logarithm of input data in mode taylor.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log2(x)
template <typename Value>
Value log2_taylor(Context* ctx, Value const& x)
{
    auto [g, h] = log2_dec(ctx, x);

    Value kf_log2_e = make_constant<Value>(ctx, std::numbers::log2e_v<double>, x.shape());

    Value ln_g   = log_taylor_core(ctx, g);
    Value log2_g = mul(ctx, kf_log2_e, ln_g);
    Value log2_x = add(ctx, log2_g, h);

    return log2_x;
}

/// @brief Implementation of getting the base 10 logarithm of input data in mode taylor.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log10(x)
template<typename Value>
Value log10_taylor(Context* ctx, Value const& x)
{
    auto [g, h] = log2_dec(ctx, x);

    Value kf_log10_2 = make_constant<Value>(ctx, std::log10(2), x.shape());
    Value kf_log10_e = make_constant<Value>(ctx, std::numbers::log10e_v<double>, x.shape());

    Value ln_g = log_taylor_core(ctx, g);
    Value log10_d = mul(ctx, kf_log10_e, ln_g);

    Value log10_x = add(ctx, log10_d, mul(ctx, kf_log10_2, h));

    return log10_x;
}

} // namespace detail

/******************************** api ********************************/

/// @brief Get the natural logarithm of input data.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log(x)
template <typename Value>
Value log(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_log_mode)
    {
    case Config::FXP_LOG_TAYLOR: return detail::log_taylor(ctx, x);
    default: throw std::runtime_error("unknown log mode");
    }
}

/// @brief Get the base 2 logarithm of input data.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log2(x)
template <typename Value>
Value log2(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_log_mode)
    {
    case Config::FXP_LOG_TAYLOR: return detail::log2_taylor(ctx, x);
    default: throw std::runtime_error("unknown log mode");
    }
}

/// @brief Get the base 10 logarithm of input data.
/// @param ctx The calculation settings
/// @param x The input Value to be logarithmized
/// @return The result, log10(x)
template <typename Value>
Value log10(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_log_mode)
    {
    case Config::FXP_LOG_TAYLOR: return detail::log10_taylor(ctx, x);
    default: throw std::runtime_error("unknown log mode");
    }
}

} // namespace pppu
