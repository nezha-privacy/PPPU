#pragma once

#include <concepts>
#include <vector>
#include <span>

#include "context/context.hpp"

namespace pppu
{

/// @brief Input a private value as a share value.
/// @param ctx The calculation settings
/// @param in The private input Value object
/// @return The share Value
template <typename Value>
Value input(Context* ctx, Value const& in);

/// @brief Open a share value as a public value.
/// @param ctx The calculation settings
/// @param in The share input Value object
/// @return The public Value
template <typename Value>
Value open(Context* ctx, Value const& in);

/// @brief Get the oppisite number of elements in the array of input Value.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The netitive Value, - in
template <typename Value>
Value neg(Context* ctx, Value const& in);

/// @brief Add two Value element by element.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value add(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Subtract two Value element by element.
/// @param ctx The calculation settings
/// @param lhs Minuend
/// @param rhs Subtrahend
/// @return The difference, lhs - rhs
template <typename Value>
Value sub(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Multiply two Value element by element.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value mul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Get the square of input Value element by element.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value square(Context* ctx, Value const& in);

/// @brief Matrix multiply two Value.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value matmul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Get the most significant bit. 
/// @details If it is a negative number, set the corresponding bit to 1.
///          If it is a positive number or 0, set the corresponding bit to 0.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value msb(Context* ctx, Value const& in);

/// @brief Determine whether it is 0 element by element, set the corresponding bit to 1 if true.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value eqz(Context* ctx, Value const& in);

/// @brief Get the sign bit.
/// @details If it is a negative number, set the corresponding bit to -1.
///          If it is a positive number or 0, set the corresponding bit to 1.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> -1, x >= 0 -> 1
template <typename Value>
Value sign(Context* ctx, Value const& in);

/// @brief Get the absolute values of elements in the array of the input Value.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, abs(x)
template <typename Value>
Value abs(Context* ctx, Value const& in);

/// @brief Decompose decimal values in input Value into binary bits stored in a vector of Value.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of x
template <typename Value>
std::vector<Value> bitdec(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Decompose decimal values in input Value into binary bits stored in a vector of Value.
///        However, only record the highest bit in vector, others are set to 0.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return Lower n bits decomposition of bitfloor(x)
template <typename Value>
std::vector<Value> h1bitdec(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Compose bit decomposed Value to a decimal Value.
/// @param ctx The calculation settings
/// @param in The input bit decomposed Value
/// @param fracbits Fixed point number decimal point position
/// @return The composed Value
template <typename Value>
Value bitcomp(Context* ctx, std::span<const Value> in, std::size_t fracbits);

} // namespace pppu
