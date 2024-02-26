#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Intermediate processing function of input for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value f_input(Context* ctx, Value const& in);

/// @brief Intermediate processing function of open for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value f_open(Context* ctx, Value const& in);

/// @brief Intermediate processing function of negation for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The negative Value, - in
template <typename Value>
Value f_neg(Context* ctx, Value const& in);

/// @brief Intermediate processing function of addition for floating-point numbers.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value f_add(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Intermediate processing function of multiplication for floating-point numbers.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value f_mul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Intermediate processing function of square for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value f_square(Context* ctx, Value const& in);

/// @brief Intermediate processing function of matrix multiplication for floating-point numbers.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value f_matmul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Intermediate processing function of most significant bit for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value f_msb(Context* ctx, Value const& in);

/// @brief Intermediate processing function of equal to zero for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value f_eqz(Context* ctx, Value const& in);

/// @brief Intermediate processing function of bit decomposition for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of x
template <typename Value>
std::vector<Value> f_bitdec(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Intermediate processing function of highest bit decomposition for floating-point numbers.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of bitfloor(x)
template <typename Value>
std::vector<Value> f_h1bitdec(Context* ctx, Value const& in, std::size_t nbits);

} // namespace pppu
