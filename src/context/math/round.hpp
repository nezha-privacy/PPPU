#pragma once

#include "round.h"

#include <bit>
#include <string>

namespace pppu
{

/// @brief Record the fracbits of input Value x.
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @return The result, x.fracbits()
template <typename Value>
Value fraction(Context* ctx, Value const& x)
{
    Value ki_0 = make_constant<Value>(ctx, 0, x.shape());

    if(x.fracbits() == 0)    return ki_0;

    std::vector<Value> x_frac_bitdec = bitdec(ctx, x, x.fracbits());

    Value x_frac = bitcomp(ctx, std::span<const Value>(x_frac_bitdec), x.fracbits());

    return x_frac;
}

/// @brief Round a number to an integer (round down).
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @param keep_fracbits Whether to preserve fracbits after converting to integers
/// @return The result, floor(x)
template <typename Value>
Value floor(Context* ctx, Value const& x, bool keep_fracbits)
{
    if(x.fracbits() == 0)    return x;

    if(keep_fracbits)
    {
        Value x_floor = sub(ctx, x, fraction(ctx, x));
        return x_floor;
    }
    else
    {
        // use trunc protocol on x_floor might not lead to correct result
        // because it might introduce truncation error
        // which is not negligible in this case
        // use bitdec for integer part instead
        throw std::runtime_error("TODO");
        int64_t intbits = 40;  // TODO: set intbits to a proper value

        auto x_bitdec     = bitdec(ctx, x, x.fracbits() + intbits);
        auto x_bitdec_int = std::span<const Value>(x_bitdec).last(intbits);

        Value x_floor = bitcomp(ctx, x_bitdec_int, 0);

        return x_floor;
    }
}

/// @brief Round a number to an integer (round up).
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @param keep_fracbits Whether to preserve fracbits after converting to integers
/// @return The result, ceil(x)
template <typename Value>
Value ceil(Context* ctx, Value const& x, bool keep_fracbits)
{
    if(x.fracbits() == 0)    return x;

    Value kf_1       = make_constant<Value>(ctx, 1, x.shape(), x.fracbits());
    Value kf_epsilon = make_constant<Value>(ctx, 1, x.shape()).set_fracbits(x.fracbits(), true);

    return floor(ctx, add(ctx, x, sub(ctx, kf_1, kf_epsilon)), keep_fracbits );
}

/// @brief Round a number to an integer (half adjust).
/// @param ctx The calculation settings
/// @param x The input Value object to be rounded
/// @param keep_fracbits Whether to preserve fracbits after converting to integers
/// @return The result, [x]
template <typename Value>
Value round(Context* ctx, Value const& x, bool keep_fracbits)
{
    if(x.fracbits() == 0)    return x;

    // round(x) = floor(x+0.5) when x > 0
    // round(x) is odd function, so
    // 1. round(x) = -round(-x)
    // 2. round(x) = sign(x) * round( sign(x)*x ) = sign(x) * round( abs(x) )

    Value kf_0_5 = make_constant<Value>(ctx, 0.5, x.shape(), 1);

    Value x_sign = sign(ctx, x);
    Value x_abs  = mul(ctx, x, x_sign);
    Value x_abs_round = floor(ctx, add(ctx, kf_0_5, x_abs), keep_fracbits);
    Value x_round = mul(ctx, x_sign, x_abs_round);

    return x_round;
}

/// @brief Perform modulo operation on Value x.
/// @param ctx The calculation settings
/// @param x The input Value object to be moded
/// @param modules The modulus number, needs to be a integer or floating point number
/// @return The result, x mod m
template <typename Value, typename T>
requires ( std::integral<T> || std::floating_point<T> )
Value mod(Context* ctx, Value const& x, T modulus)
{
    if(modulus <= 0)
    {
        std::string err = "invalid modulus " + std::to_string(modulus);
        throw std::invalid_argument(err);
    }

    using unsigned_int_type = unsigned long long;
    static_assert( sizeof(unsigned_int_type) >= sizeof(T) );

    Value ans;
    if ( std::integral<T> && std::has_single_bit(unsigned_int_type(modulus)) )
    {
        int64_t nbits = std::countr_zero(unsigned_int_type(modulus)) + x.fracbits();
        if(nbits == 0)
            return make_constant<Value>(ctx, 0, x.shape());

        std::vector<Value> x_bitdec = bitdec(ctx, x, nbits);
        ans = bitcomp(ctx, std::span<const Value>(x_bitdec), x.fracbits());
    }
    else
    {
        Value mod      = make_constant<Value>(ctx, modulus, x.shape());
        Value mod_recp = make_constant<Value>(ctx, 1.0 / modulus, x.shape());
        Value t = mul(ctx, mod, floor(ctx, mul(ctx, x, mod_recp)));
        ans = sub(ctx, x, t);
    }
    return ans;
}

} // namespace pppu
