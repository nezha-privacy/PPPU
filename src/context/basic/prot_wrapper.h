#pragma once

#include <cstdlib>
#include <vector>

#include "context/context.hpp"
#include "context/value.hpp"
#include "ndarray/array_ref.hpp"
#include "ndarray/ndarray_ref.hpp"
#include "ndarray/tools.hpp"

namespace pppu
{

/// @brief Determine whether the input meets the requirements of matrix multiplication.
template <typename Protocol, typename pdtype, typename sdtype>
concept protWithMethodMatmul = requires(Protocol prot, core::ArrayRef<pdtype> p, core::ArrayRef<sdtype> s, int64_t M, int64_t N, int64_t K)
{
    { prot.matmul_pp( p, p, M, N, K ) } -> std::same_as< core::ArrayRef<pdtype> >;
    { prot.matmul_ps( p, s, M, N, K ) } -> std::same_as< core::ArrayRef<sdtype> >;
    { prot.matmul_sp( s, p, M, N, K ) } -> std::same_as< core::ArrayRef<sdtype> >;
    { prot.matmul_ss( s, s, M, N, K ) } -> std::same_as< core::ArrayRef<sdtype> >;
};

/// @brief Determine whether the input meets the requirements of square.
template <typename Protocol, typename pdtype, typename sdtype>
concept protWithMethodSquare = requires(Protocol prot, core::ArrayRef<pdtype> p, core::ArrayRef<sdtype> s)
{
    { prot.square_p( p ) } -> std::same_as< core::ArrayRef<pdtype> >;
    { prot.square_s( s ) } -> std::same_as< core::ArrayRef<sdtype> >;
};

/// @brief Determine whether the input meets the requirements of equal to zero.
template <typename Protocol, typename pdtype, typename sdtype>
concept protWithMethodEqz = requires(Protocol prot, core::ArrayRef<pdtype> p, core::ArrayRef<sdtype> s)
{
    { prot.eqz_p( p ) } -> std::same_as< core::ArrayRef<pdtype> >;
    { prot.eqz_s( s ) } -> std::same_as< core::ArrayRef<sdtype> >;
};

/// @brief Redirection function of input based on protocol for private input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value _input_p(Context* ctx, Value const& in);

/// @brief Redirection function of open based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value _open_s(Context* ctx, Value const& in);

/// @brief Redirection function of negation based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The netitive Value, - in
template <typename Value>
Value _neg_p(Context* ctx, Value const& in);

/// @brief Redirection function of negation based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The netitive Value, - in
template <typename Value>
Value _neg_s(Context* ctx, Value const& in);

/// @brief Redirection function of addition based on protocol for (share, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value _add_pp(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of addition based on protocol for (share, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value _add_sp(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of addition based on protocol for (plain, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value _add_ss(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of multiplication based on protocol for (plain, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value _mul_pp(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of multiplication based on protocol for (share, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value _mul_sp(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of multiplication based on protocol for (share, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value _mul_ss(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of matrix multiplication based on protocol for (plain, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_pp(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of matrix multiplication based on protocol for (share, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_sp(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of matrix multiplication based on protocol for (plain, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_ps(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of matrix multiplication based on protocol for (share, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_ss(Context* ctx, Value const& lhs, Value const& rhs);

/// @brief Redirection function of square based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value _square_p(Context* ctx, Value const& in);

/// @brief Redirection function of square based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value _square_s(Context* ctx, Value const& in);

/// @brief Redirection function of most significant bit based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value _msb_s(Context* ctx, Value const& in);

/// @brief Redirection function of most significant bit based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value _msb_p(Context* ctx, Value const& in);

/// @brief Redirection function of equal to zero functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value _eqz_s(Context* ctx, Value const& in);

/// @brief Redirection function of equal to zero functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value _eqz_p(Context* ctx, Value const& in);

/// @brief Redirection function of left bit shift functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x << nbits
template <typename Value>
Value _lshift_s(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of left bit shift functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x << nbits
template <typename Value>
Value _lshift_p(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of truncation functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x >> nbits
template <typename Value>
Value _trunc_s(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of truncation functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x >> nbits
template <typename Value>
Value _trunc_p(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of bit decomposition functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _bitdec_p(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of bit decomposition functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _bitdec_s(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of highest bit decomposition functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _h1bitdec_p(Context* ctx, Value const& in, std::size_t nbits);

/// @brief Redirection function of highest bit decomposition functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _h1bitdec_s(Context* ctx, Value const& in, std::size_t nbits);

} // namespace pppu
