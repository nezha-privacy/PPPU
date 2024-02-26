#pragma once

#include "basic.h"

#include "fxp.hpp"

#include "factory.hpp"

namespace pppu
{

/// @brief Input a private value as a share value.
/// @param ctx The calculation settings
/// @param in The private input Value object
/// @return The share Value
template <typename Value>
Value input(Context* ctx, Value const& in)
{
    return f_input(ctx, in);
}

/// @brief Open a share value as a public value.
/// @param ctx The calculation settings
/// @param in The share input Value object
/// @return The public Value
template <typename Value>
Value open(Context* ctx, Value const& in)
{
    return f_open(ctx, in);
}

/// @brief Get the oppisite number of elements in the array of input Value.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The netitive Value, - in
template <typename Value>
Value neg(Context* ctx, Value const& in)
{
    return f_neg(ctx, in);
}

/// @brief Add two Value element by element.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value add(Context* ctx, Value const& lhs, Value const& rhs)
{
    return f_add(ctx, lhs, rhs);
}

/// @brief Subtract two Value element by element.
/// @param ctx The calculation settings
/// @param lhs Minuend
/// @param rhs Subtrahend
/// @return The difference, lhs - rhs
template <typename Value>
Value sub(Context* ctx, Value const& lhs, Value const& rhs)
{
    return f_add(ctx, lhs, f_neg(ctx, rhs));
}

/// @brief Multiply two Value element by element.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value mul(Context* ctx, Value const& lhs, Value const& rhs)
{
    return f_mul(ctx, lhs, rhs);
}

/// @brief Matrix multiply two Value.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    return f_matmul(ctx, lhs, rhs);
}

/// @brief Get the square of input Value element by element.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value square(Context* ctx, Value const& in)
{
    using Protocol = typename Value::Protocol;
    using pdtype = typename Value::PlainType::value_type;
    using sdtype = typename Value::ShareType::value_type;

    if constexpr ( protWithMethodSquare<Protocol, pdtype, sdtype> )
    {
        return f_square(ctx, in);
    }
    else
    {
        return f_mul(ctx, in, in);
    }
}

/// @brief Get the most significant bit.
/// @details If it is a negative number, set the corresponding bit to 1.
///          If it is a positive number or 0, set the corresponding bit to 0.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value msb(Context* ctx, Value const& in)
{
    return f_msb(ctx, in);
}

/// @brief Determine whether it is 0 element by element, set the corresponding bit to 1 if true.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value eqz(Context* ctx, Value const& x)
{
    using Protocol = typename Value::Protocol;
    using pdtype = typename Value::PlainType::value_type;
    using sdtype = typename Value::ShareType::value_type;

    if constexpr ( protWithMethodEqz<Protocol, pdtype, sdtype> )
    {
        return f_eqz(ctx, x);
    }
    else
    {
        Value ki_1 = make_constant<Value>(ctx, 1, x.shape());
        Value negative = msb(ctx, x);
        Value positive = msb(ctx, neg(ctx, x));
        Value non_negative = sub(ctx, ki_1, negative);
        Value non_positive = sub(ctx, ki_1, positive);

        Value eqz = mul(ctx, non_negative, non_positive);
        return eqz;
    }
}

/// @brief Get the sign bit.
/// @details If it is a negative number, set the corresponding bit to -1.
///          If it is a positive number or 0, set the corresponding bit to 1.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> -1, x >= 0 -> 1
template <typename Value>
Value sign(Context* ctx, Value const& x)
{
    Value ki_1 = make_constant<Value>(ctx, 1, x.shape());
    Value ki_2 = make_constant<Value>(ctx, 2, x.shape());

    Value x_msb  = msb(ctx, x);
    Value x_sign = sub(ctx, ki_1, mul(ctx, ki_2, x_msb));

    return x_sign;
}

/// @brief Get the absolute values of elements in the array of the input Value.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, abs(x)
template <typename Value>
Value abs(Context* ctx, Value const& x)
{
    Value x_sign = sign(ctx, x);
    Value x_abs  = mul (ctx, x, x_sign);
    return x_abs;
}

/// @brief Decompose decimal values in input Value into binary bits stored in a vector of Value.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of x
template <typename Value>
std::vector<Value> bitdec(Context* ctx, Value const& in, std::size_t nbits)
{
    return f_bitdec(ctx, in, nbits);
}

/// @brief Decompose decimal values in input Value into binary bits stored in a vector of Value.
///        However, only record the highest bit in vector, others are set to 0.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of bitfloor(x)
template <typename Value>
std::vector<Value> h1bitdec(Context* ctx, Value const& in, std::size_t nbits)
{
    return f_h1bitdec(ctx, in, nbits);
}

/// @brief Compose bit decomposed Value to a decimal Value.
/// @param ctx The calculation settings
/// @param bitdec The input bit decomposed Value
/// @param fracbits Fixed point number decimal point position
/// @return The composed Value
template <typename Value>
Value bitcomp(Context* ctx, std::span<const Value> bitdec, std::size_t fracbits)
{

    if(bitdec.size() == 0) {
        throw std::invalid_argument("bit composition with zero input bits");
    }

    int64_t nbits = bitdec.size();

    auto const& shape = bitdec.front().shape();

    Value ki_0 = make_constant<Value>(ctx, 0, shape);
    Value ki_1 = make_constant<Value>(ctx, 1, shape);
    Value ki_2 = make_constant<Value>(ctx, 2, shape);

    Value ans    = ki_0;
    Value ki_2_i = ki_1;
    for(int64_t i = 0; i < nbits; ++i) {
        assert(bitdec[i].fracbits() == 0);

        ans = add(ctx, ans, mul(ctx, bitdec[i], ki_2_i));
        ki_2_i = mul(ctx, ki_2_i, ki_2);
    }

    return ans.set_fracbits(fracbits, true);
}

} // namespace pppu
