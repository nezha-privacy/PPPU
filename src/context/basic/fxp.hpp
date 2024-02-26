#pragma once

#include "fxp.h"

#include "raw.hpp"

namespace pppu
{

/// @brief Intermediate processing function of input for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value f_input(Context* ctx, Value const& in)
{
    return r_input(ctx, in).set_fracbits(in.fracbits() , true);
}

/// @brief Intermediate processing function of open for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value f_open(Context* ctx, Value const& in)
{
    return r_open(ctx, in).set_fracbits(in.fracbits() , true);
}

/// @brief Intermediate processing function of negation for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The negative Value, - in
template <typename Value>
Value f_neg(Context* ctx, Value const& in)
{
    return r_neg(ctx, in).set_fracbits(in.fracbits() , true);
}

/// @brief Intermediate processing function of addition for floating-point numbers.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value f_add(Context* ctx, Value const& _lhs, Value const& _rhs)
{
    Value lhs = _lhs;
    Value rhs = _rhs;

    int64_t fracbits = std::max( lhs.fracbits(), rhs.fracbits() );

    if( lhs.fracbits() != fracbits )
        lhs = r_lshift( ctx, lhs, fracbits - lhs.fracbits() );
    if( rhs.fracbits() != fracbits )
        rhs = r_lshift( ctx, rhs, fracbits - rhs.fracbits() );

    return r_add(ctx, lhs, rhs).set_fracbits(fracbits , true);
}

/// @brief Intermediate processing function of multiplication for floating-point numbers.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value f_mul(Context* ctx, Value const& lhs, Value const& rhs)
{
    int64_t fracbits = lhs.fracbits() + rhs.fracbits();

    Value ans = r_mul(ctx, lhs, rhs);

    if( fracbits > ctx->fxp_fracbits() ) {
        ans = r_trunc( ctx, ans, fracbits - ctx->fxp_fracbits() );
        fracbits = ctx->fxp_fracbits();
    }

    return ans.set_fracbits(fracbits , true);
}

/// @brief Intermediate processing function of square for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value f_square(Context* ctx, Value const& in)
{
    int64_t fracbits = 2 * in.fracbits();

    Value ans = r_square(ctx, in);

    if( fracbits > ctx->fxp_fracbits() ) {
        ans = r_trunc( ctx, ans, fracbits - ctx->fxp_fracbits() );
        fracbits = ctx->fxp_fracbits();
    }

    return ans.set_fracbits(fracbits , true);
}

/// @brief Intermediate processing function of matrix multiplication for floating-point numbers.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value f_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    int64_t fracbits = lhs.fracbits() + rhs.fracbits();

    Value ans = r_matmul(ctx, lhs, rhs);

    if( fracbits > ctx->fxp_fracbits() ) {
        ans = r_trunc( ctx, ans, fracbits - ctx->fxp_fracbits() );
        fracbits = ctx->fxp_fracbits();
    }

    return ans.set_fracbits(fracbits , true);
}

/// @brief Intermediate processing function of most significant bit for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value f_msb(Context* ctx, Value const& in)
{
    return r_msb(ctx, in).set_fracbits(0, true);
}

/// @brief Intermediate processing function of equal to zero for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value f_eqz(Context* ctx, Value const& in)
{
    return r_eqz(ctx, in).set_fracbits(0, true);
}

/// @brief Intermediate processing function of bit decomposition for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of x
template <typename Value>
std::vector<Value> f_bitdec(Context* ctx, Value const& in, std::size_t nbits)
{
    auto ans = r_bitdec(ctx, in, nbits);
    for(auto& x: ans)   x.set_fracbits(0, true);
    return ans;
}

/// @brief Intermediate processing function of highest bit decomposition for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of bitfloor(x)
template <typename Value>
std::vector<Value> f_h1bitdec(Context* ctx, Value const& in, std::size_t nbits)
{
    auto ans = r_h1bitdec(ctx, in, nbits);
    for(auto& x: ans)   x.set_fracbits(0, true);
    return ans;
}

} // namespace pppu
