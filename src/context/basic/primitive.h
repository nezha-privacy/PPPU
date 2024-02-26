#pragma once

#include "context/context.hpp"

namespace pppu
{

/// @brief Redirection function of input for private input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value Pr_input(Context* ctx, Value const& in);

/// @brief Redirection function of open for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value Sh_open(Context* ctx, Value const& in);

/// @brief Redirection function of negation function for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value Pb_neg(Context* ctx, Value const& in);
template <typename Value>
Value Pr_neg(Context* ctx, Value const& in);
template <typename Value>
Value Sh_neg(Context* ctx, Value const& in);

/// @brief Redirection function of addition function for (public, public), (private, public), (share, public), 
///        (private, private), (share, private) and (share, share) input pair respectively.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The result Value
template <typename Value>
Value PbPb_add(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrPb_add(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShPb_add(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrPr_add(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShPr_add(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShSh_add(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of multiplication function for (public, public), (private, public), (share, public), 
///        (private, private), (share, private) and (share, share) input pair respectively.
/// @param ctx The calculation settings
/// @param lhs First multipler
/// @param rhs Second multipler
/// @return The result Value
template <typename Value>
Value PbPb_mul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrPb_mul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShPb_mul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrPr_mul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShPr_mul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShSh_mul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of matrix multiplication function for (public, public), (private, public), (share, public), 
///        (private, private), (share, private) and (share, share) input pair respectively.
/// @param ctx The calculation settings
/// @param lhs First matrix multipler
/// @param rhs Second matrix multipler
/// @return The result Value
template <typename Value>
Value PbPb_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PbPr_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PbSh_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrPb_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrPr_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value PrSh_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShPb_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShPr_matmul(Context* ctx, Value const& lhs, Value const& rhs);
template <typename Value>
Value ShSh_matmul(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of square function for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value Pb_square(Context* ctx, Value const& in);
template <typename Value>
Value Pr_square(Context* ctx, Value const& in);
template <typename Value>
Value Sh_square(Context* ctx, Value const& in);

/// @brief Redirection function of most significant bit function for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value Pb_msb(Context* ctx, Value const& in);
template <typename Value>
Value Pr_msb(Context* ctx, Value const& in);
template <typename Value>
Value Sh_msb(Context* ctx, Value const& in);

/// @brief Redirection function of equal to zero function for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
template <typename Value>
Value Pb_eqz(Context* ctx, Value const& in);
template <typename Value>
Value Pr_eqz(Context* ctx, Value const& in);
template <typename Value>
Value Sh_eqz(Context* ctx, Value const& in);

/// @brief Redirection function of left bit shift functions for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result Value
template <typename Value>
Value Pb_lshift(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
Value Pr_lshift(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
Value Sh_lshift(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of truncation functions for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result Value
template <typename Value>
Value Pb_trunc(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
Value Pr_trunc(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
Value Sh_trunc(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of bit decomposition functions public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
template <typename Value>
std::vector<Value> Pb_bitdec(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
std::vector<Value> Pr_bitdec(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
std::vector<Value> Sh_bitdec(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of highest bit decomposition functions public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
template <typename Value>
std::vector<Value> Pb_h1bitdec(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
std::vector<Value> Pr_h1bitdec(Context* ctx, Value const& in, std::size_t nbits);
template <typename Value>
std::vector<Value> Sh_h1bitdec(Context* ctx, Value const& in, std::size_t nbits);

} // namespace pppu
