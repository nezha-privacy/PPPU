#pragma once

#include <cstdlib>

#include "context/context.hpp"

namespace pppu
{

/// @brief Redirection function of input based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value r_input(Context* ctx, Value const& in);

/// @brief Redirection function of open based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value r_open(Context* ctx, Value const& in);

/// @brief Redirection function of negation function based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value r_neg(Context* ctx, Value const& in);

/// @brief Redirection function of addition function based on visibility.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The result Value
template <typename Value>
Value r_add(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of multiplication function based on visibility.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The result Value
template <typename Value>
Value r_mul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of square function based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value r_square(Context* ctx, Value const& in);

/// @brief Redirection function of matrix multiplication function based on visibility.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The result Value
template <typename Value>
Value r_matmul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of left bit shift function based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result Value
template <typename Value>
Value r_lshift(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of truncation function based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result Value
template <typename Value>
Value r_trunc(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of most significant bit function based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value r_msb(Context* ctx, Value const& in);

/// @brief Redirection function of equal to zero function based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value r_eqz(Context* ctx, Value const& in);

/// @brief Redirection function of bit decomposition functions based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
template <typename Value>
std::vector<Value> r_bitdec(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of highest bit decomposition functions based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
template <typename Value>
std::vector<Value> r_h1bitdec(Context* ctx, Value const& in, std::size_t nbits);

} // namespace pppu
