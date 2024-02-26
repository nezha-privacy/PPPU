#pragma once

#include "prot_wrapper.h"

#include "ndarray/util.hpp"

namespace pppu
{

/// @brief Redirection function of input based on protocol for private input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value _input_p(Context* ctx, Value const& in)
{
    assert( in.is_private() );

    using pdtype = typename Value::PlainType::value_type;

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    if(ctx->pid() == in.owner())   // local private input
    {
        auto fin = flatten( in.data_p() );
        auto fans = prot->input_p( fin );
        ans.assign_s( unflatten( fans, shape ) );
    }
    else  // remote input
    {
        int64_t numel = core::detail::calcNumel(shape);
        auto fans = prot->template input_p<pdtype>( in.owner(), numel );
        ans.assign_s( unflatten( fans, shape ) );
    }

    return ans;
}

/// @brief Redirection function of open based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value _open_s(Context* ctx, Value const& in)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->open_s( fin );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of negation based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The netitive Value, - in
template <typename Value>
Value _neg_p(Context* ctx, Value const& in)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_p() );
    auto fans = prot->neg_p( fin );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of negation based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The netitive Value, - in
template <typename Value>
Value _neg_s(Context* ctx, Value const& in)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->neg_s( fin );
    ans.assign_s( unflatten(fans, in.shape()) );

    return ans;
}

/// @brief Redirection function of addition based on protocol for (share, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value _add_ss(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_share() && rhs.is_share() );
    assert( lhs.shape() == rhs.shape() );


    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = lhs.shape();

    Value ans;

    auto flhs = flatten( lhs.data_s() );
    auto frhs = flatten( rhs.data_s() );
    auto fans = prot->add_ss( flhs, frhs );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of addition based on protocol for (share, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value _add_sp(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_share() && rhs.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = lhs.shape();

    Value ans;

    auto flhs = flatten( lhs.data_s() );
    auto frhs = flatten( rhs.data_p() );
    auto fans = prot->add_sp( flhs, frhs );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of addition based on protocol for (plain, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First addend
/// @param rhs Second addend
/// @return The sum, lhs + rhs
template <typename Value>
Value _add_pp(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_plain() && rhs.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = lhs.shape();

    Value ans;

    auto flhs = flatten( lhs.data_p() );
    auto frhs = flatten( rhs.data_p() );
    auto fans = prot->add_pp( flhs, frhs );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of multiplication based on protocol for (plain, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value _mul_pp(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_plain() && rhs.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = lhs.shape();

    Value ans;

    auto flhs = flatten( lhs.data_p() );
    auto frhs = flatten( rhs.data_p() );
    auto fans = prot->mul_pp( flhs, frhs );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of multiplication based on protocol for (share, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value _mul_sp(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_share() && rhs.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = lhs.shape();

    Value ans;

    auto flhs = flatten( lhs.data_s() );
    auto frhs = flatten( rhs.data_p() );
    auto fans = prot->mul_sp( flhs, frhs );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of multiplication based on protocol for (share, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First multiplier
/// @param rhs Second multiplier
/// @return The product, lhs * rhs
template <typename Value>
Value _mul_ss(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_share() && rhs.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = lhs.shape();

    Value ans;

    auto flhs = flatten( lhs.data_s() );
    auto frhs = flatten( rhs.data_s() );
    auto fans = prot->mul_ss( flhs, frhs );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of matrix multiplication based on protocol for (plain, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_pp(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_plain() && rhs.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    auto [M, N, K] = core::detail::deduceMatmulShape(lhs.shape(), rhs.shape());

    Value ans;

    auto flhs = flatten( lhs.data_p() );
    auto frhs = flatten( rhs.data_p() );
    auto fans = prot->matmul_pp( flhs, frhs, M, N, K );
    ans.assign_p( unflatten(fans, {M, K}) );

    return ans;
}

/// @brief Redirection function of matrix multiplication based on protocol for (share, plain) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_sp(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_share() && rhs.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    auto [M, N, K] = core::detail::deduceMatmulShape(lhs.shape(), rhs.shape());

    Value ans;

    auto flhs = flatten( lhs.data_s() );
    auto frhs = flatten( rhs.data_p() );
    auto fans = prot->matmul_sp( flhs, frhs, M, N, K );
    ans.assign_s( unflatten(fans, {M, K}) );

    return ans;
}

/// @brief Redirection function of matrix multiplication based on protocol for (plain, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_ps(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_plain() && rhs.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    auto [M, N, K] = core::detail::deduceMatmulShape(lhs.shape(), rhs.shape());

    Value ans;

    auto flhs = flatten( lhs.data_p() );
    auto frhs = flatten( rhs.data_s() );
    auto fans = prot->matmul_ps( flhs, frhs, M, N, K );
    ans.assign_s( unflatten(fans, {M, K}) );

    return ans;
}

/// @brief Redirection function of matrix multiplication based on protocol for (share, share) input pair.
/// @param ctx The calculation settings
/// @param lhs First matrix multiplier
/// @param rhs Second matrix multiplier
/// @return The matrix product, (lhsrhs)
template <typename Value>
Value _matmul_ss(Context* ctx, Value const& lhs, Value const& rhs)
{
    assert( lhs.is_share() && rhs.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    auto [M, N, K] = core::detail::deduceMatmulShape(lhs.shape(), rhs.shape());

    Value ans;

    auto flhs = flatten( lhs.data_s() );
    auto frhs = flatten( rhs.data_s() );
    auto fans = prot->matmul_ss( flhs, frhs, M, N, K );
    ans.assign_s( unflatten(fans, {M, K}) );

    return ans;
}

/// @brief Redirection function of square based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value _square_p(Context* ctx, Value const& in)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_p() );
    auto fans = prot->square_p( fin );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of square based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The product, in ^ 2
template <typename Value>
Value _square_s(Context* ctx, Value const& in)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->square_s( fin );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of most significant bit based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value _msb_s(Context* ctx, Value const& in)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->msb_s( fin );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of most significant bit based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x < 0 -> 1, x >= 0 -> 0
template <typename Value>
Value _msb_p(Context* ctx, Value const& in)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_p() );
    auto fans = prot->msb_p( fin );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of left bit shift functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x << nbits
template <typename Value>
Value _lshift_s(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->lshift_s( fin, nbits );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of left bit shift functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x << nbits
template <typename Value>
Value _lshift_p(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_p() );
    auto fans = prot->lshift_p( fin, nbits );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of truncation functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x >> nbits
template <typename Value>
Value _trunc_s(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->trunc_s( fin, nbits );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of truncation functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result, x >> nbits
template <typename Value>
Value _trunc_p(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_p() );
    auto fans = prot->trunc_p( fin, nbits );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of equal to zero functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value _eqz_s(Context* ctx, Value const& in)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_s() );
    auto fans = prot->eqz_s( fin );
    ans.assign_s( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of equal to zero functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result, x = 0 -> 1, x != 0 -> 0
template <typename Value>
Value _eqz_p(Context* ctx, Value const& in)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();
    auto const& shape = in.shape();

    Value ans;

    auto fin = flatten( in.data_p() );
    auto fans = prot->eqz_p( fin );
    ans.assign_p( unflatten(fans, shape) );

    return ans;
}

/// @brief Redirection function of bit decomposition functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _bitdec_p(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    std::vector<Value> ans;

    for(auto& x: prot->bitdec_p( flatten( in.data_p() ), nbits )) {
        ans.emplace_back(
            Value{}.assign_p( unflatten(x, in.shape()) )
        );
    }

    return ans;
}

/// @brief Redirection function of bit decomposition functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _bitdec_s(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    std::vector<Value> ans;

    for(auto& x: prot->bitdec_s( flatten( in.data_s() ), nbits )) {
        ans.emplace_back(
            Value{}.assign_s( unflatten(x, in.shape()) )
        );
    }

    return ans;
}

/// @brief Redirection function of highest bit decomposition functions based on protocol for plain input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _h1bitdec_p(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_plain() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    std::vector<Value> ans;

    for(auto& x: prot->h1bitdec_p( flatten( in.data_p() ), nbits )) {
        ans.emplace_back(
            Value{}.assign_p( unflatten(x, in.shape()) )
        );
    }

    return ans;
}

/// @brief Redirection function of highest bit decomposition functions based on protocol for share input.
/// @param ctx The calculation settings
/// @param in The input decimal Value object
/// @param nbits Maximum decomposed binary bits
/// @return The decomposed vector of Value
template <typename Value>
std::vector<Value> _h1bitdec_s(Context* ctx, Value const& in, std::size_t nbits)
{
    assert( in.is_share() );

    auto* prot = ctx->prot<typename Value::Protocol>();

    std::vector<Value> ans;

    for(auto& x: prot->h1bitdec_s( flatten( in.data_s() ), nbits )) {
        ans.emplace_back(
            Value{}.assign_s( unflatten(x, in.shape()) )
        );
    }

    return ans;
}

} // namespace pppu
