#pragma once

#include <cmath>
#include <ranges>
#include <numbers>
#include <vector>

#include "exp.h"

#include "pow.hpp"
#include "polynomial.hpp"

namespace pppu
{

namespace detail
{

/******************************** euler ********************************/

/// @brief Implementation of getting the x power of the base of natural logarithms in mode euler.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp(x) = (1+x/N)^N, N comes from ctx
template <typename Value>
Value exp_euler(Context* ctx, Value const& x)
{
    int64_t n_iters = ctx->config()->fxp_exp_iters;

    if( n_iters > 62 || n_iters > ctx->fxp_fracbits() )
        throw std::runtime_error("exp euler iterations exceeds limit");

    int64_t N = (1 << n_iters);

    Value kf_1_0 = make_constant<Value>(ctx, 1.0, x.shape());
    Value kf_1_N = make_constant<Value>(ctx, 1.0/N, x.shape());

    Value t = add(ctx, kf_1_0, mul(ctx, x, kf_1_N));

    return pow(ctx, t, N);
}

/// @brief Implementation of getting the x power of 2.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp2(x) = exp( ln2 * x )
template <typename Value>
Value exp2_euler(Context* ctx, Value const& x)
{
    Value kf_ln_2 = make_constant<Value>(ctx, std::numbers::ln2_v<double>, x.shape());
    return exp_euler(ctx, mul(ctx, kf_ln_2, x));
}

/******************************** taylor ********************************/

/// @brief Core function of getting the x power of 2 in mode taylor.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp(x)  = 1 + x + x^2/(2!) + x^3/(3!) + ..., and exp2(x) = exp( ln2 * x )
template <typename Value>
Value exp2_taylor_core(Context* ctx, Value const& x)
{
    int64_t n_iters = std::max(int64_t(2), ctx->config()->fxp_exp_iters);

    Value ki_1 = make_constant<Value>(ctx, 1, x.shape());

    std::vector<Value> coef(n_iters);
    coef[0] = ki_1;
    for(int64_t i = 1; i < n_iters; ++i) {
        // tgamma(n+1) = n!
        coef[i] = make_constant<Value>(ctx, std::pow(std::log(2), i) / std::tgamma(i+1), x.shape());
    }

    return polynomial(ctx, std::span<const Value>(coef), x);
}

/// @brief Faster power algorithm of getting the x power of 2 in mode taylor.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp2(x) is done with integer arithmetic, avoid unnecessary truncation and be faster
template <typename Value>
Value exp2_int(Context* ctx, std::span<const Value> x_bitdec)
{
    int64_t field_bits = 128;      // TODO: implement this
    int64_t intbits  = x_bitdec.size();
    int64_t sparam   = ctx->config()->fxp_security_parameter;

    // to ensure probability of truncation error is under 2^{-sparam}
    if(std::pow(2, intbits) + sparam > field_bits) {
        throw std::invalid_argument("exp input range exceeds security limit");
    }

    auto const& shape = x_bitdec.front().shape();
    Value ki_1 = make_constant<Value>(ctx, 1, shape);

    // when x > 0, we get x_bitdec = x, so raw = exp2(x)
    // when x < 0, we get x_bitdec = x + pow(2, intbits),
    // and raw = exp2(x + pow(2, intbits)) = exp2(x) * exp2(pow(2, intbits))
    Value raw = ki_1;
    Value ki_pow2 = make_constant<Value>(ctx, 2, shape);
    for(std::size_t i = 0; i < intbits; ++i) {
        Value t = conditional(ctx, x_bitdec[i], ki_1, ki_pow2);
        ki_pow2 = square(ctx, ki_pow2);
        raw = mul(ctx, raw, t);
    }

    // raw should be integer because all previous computation is done with int
    assert(raw.fracbits() == 0);

    // when x < 0, we have exp2(x) = raw >> pow(2, intbits)
    Value raw_trunc;
    int64_t fracbits = ctx->config()->fxp_fracbits;
    if(fracbits >= std::pow(2, intbits)) {
        // avoid unnecessary truncation while casting to fxp
        raw_trunc = r_lshift(ctx, raw, fracbits - std::pow(2, intbits) ).set_fracbits(fracbits);
    } else {
        raw_trunc = r_trunc(ctx, raw, std::pow(2, intbits) - fracbits ).set_fracbits(fracbits);
    }

    Value x_msb = x_bitdec.back();
    Value ans = conditional(ctx, x_msb, raw, raw_trunc);
    return ans;
}

/// @brief Implementation of getting the x power of 2 in mode taylor.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp2(x)
template <typename Value>
Value exp2_taylor(Context* ctx, Value const& x)
{
    // use fast power algorithm to compute part of fraction bits of x
    int64_t MAGIC_NUMBER = 3;

    int64_t intbits = ctx->config()->fxp_exp_intbits;
    int64_t fracbits1 = std::min(MAGIC_NUMBER, x.fracbits());
    int64_t fracbits2 = x.fracbits() - fracbits1;

    std::vector<Value> _x_bitdec = bitdec(ctx, x, intbits + x.fracbits());

    std::span x_bitdec       = std::span<const Value>(_x_bitdec);
    std::span x_bitdec_int   = x_bitdec.last(intbits);
    std::span x_bitdec_frac1 = x_bitdec.subspan(fracbits2, fracbits1);
    std::span x_bitdec_frac2 = x_bitdec.first(fracbits2);

    Value ki_1 = make_constant<Value>(ctx, 1, x.shape());

    Value exp2_z = exp2_int(ctx, x_bitdec_int);

    Value exp2_f1 = ki_1;
    for(int i = 1; i <= fracbits1; ++i) {
        Value kf_2_2_i = make_constant<Value>(ctx, std::pow(2, std::pow(2, -i)), x.shape());
        Value t = conditional(ctx, x_bitdec_frac1[fracbits1-i], ki_1, kf_2_2_i);
        exp2_f1 = mul(ctx, exp2_f1, t);
    }

    Value exp2_f2 = ki_1;
    if(fracbits2 > 0)
    {
        Value x_frac2 = bitcomp(ctx, x_bitdec_frac2, x.fracbits());
        exp2_f2 = exp2_taylor_core(ctx, x_frac2);
    }

    Value exp2_x = mul(ctx, exp2_z, mul(ctx, exp2_f1, exp2_f2));
    return exp2_x;

}

/// @brief Implementation of getting the x power of the base of natural logarithms in mode taylor.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp(x) = 2 ^ ( log2(e) * x )
template <typename Value>
Value exp_taylor(Context* ctx, Value const& x)
{
    Value kf_log2_e = make_constant<Value>(ctx, std::numbers::log2e_v<double>, x.shape());
    return exp2_taylor(ctx, mul(ctx, kf_log2_e, x));
}

} // namespace detail

/******************************** api ********************************/

/// @brief Get the x power of the base of natural logarithms.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp(x)
template <typename Value>
Value exp(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_exp_mode)
    {
    case Config::FXP_EXP_EULER:  return detail::exp_euler(ctx, x);
    case Config::FXP_EXP_TAYLOR: return detail::exp_taylor(ctx, x);
    default:                     throw std::runtime_error("unknown exp mode");
    }
}

/// @brief Get the x power of 2.
/// @param ctx The calculation settings
/// @param x The power number
/// @return The result, exp2(x)
template <typename Value>
Value exp2(Context* ctx, Value const& x)
{
    switch (ctx->config()->fxp_exp_mode)
    {
    case Config::FXP_EXP_EULER:  return detail::exp2_euler(ctx, x);
    case Config::FXP_EXP_TAYLOR: return detail::exp2_taylor(ctx, x);
    default:                     throw std::runtime_error("unknown exp mode");
    }
}

} // namespace pppu
