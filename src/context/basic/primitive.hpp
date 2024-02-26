#pragma once

#include "primitive.h"

#include "prot_wrapper.hpp"

namespace pppu
{

/// @brief Redirection function of input for private input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value Pr_input(Context* ctx, Value const& in)
{
    return _input_p(ctx, in)
        .set_visibility(Visibility::Share());
}

/// @brief Redirection function of open for share input.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value Sh_open(Context* ctx, Value const& in)
{
    return _open_s(ctx, in)
        .set_visibility(Visibility::Public());
}

/// @brief Redirection function of non-array functions for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
#define __PRIMITIVE_UNARY_FUNC_IMPLEMENT__(name)                \
template <typename Value>                                       \
Value Pb_##name(Context* ctx, Value const& in)                  \
{                                                               \
    return _##name##_p(ctx, in)                                 \
        .set_visibility(Visibility::Public());                  \
}                                                               \
                                                                \
template <typename Value>                                       \
Value Pr_##name(Context* ctx, Value const& in)                  \
{                                                               \
    if(ctx->pid() == in.owner())                                \
        return _##name##_p(ctx, in)                             \ 
            .set_visibility(Visibility::Private(in.owner()));   \
    else                                                        \
        return in;                                              \
}                                                               \
                                                                \
template <typename Value>                                       \
Value Sh_##name(Context* ctx, Value const& in)                  \
{                                                               \
    return _##name##_s(ctx, in)                                 \
        .set_visibility(Visibility::Share());                   \
}

/// @brief Redirection function of binary functions for (public, public), (private, public), (share, public), 
///        (private, private), (share, private) and (share, share) input pair respectively.
/// @param ctx The calculation settings
/// @param lhs First input
/// @param rhs Second input
/// @return The result Value
#define __PRIMITIVE_BINARY_FUNC_IMPLEMENT__(name)                   \
template <typename Value>                                           \
Value PbPb_##name(Context* ctx, Value const& lhs, Value const& rhs) \
{                                                                   \
    return _##name##_pp(ctx, lhs, rhs)                              \
        .set_visibility(Visibility::Public());                      \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value PrPb_##name(Context* ctx, Value const& lhs, Value const& rhs) \
{                                                                   \
    if(lhs.owner() == ctx->pid())                                   \
        return _##name##_pp(ctx, lhs, rhs)                          \
            .set_visibility(Visibility::Private(lhs.owner()));      \
    else                                                            \
        return lhs;                                                 \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value ShPb_##name(Context* ctx, Value const& lhs, Value const& rhs) \
{                                                                   \
    return _##name##_sp(ctx, lhs, rhs)                              \
        .set_visibility(Visibility::Share());                       \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value PrPr_##name(Context* ctx, Value const& lhs, Value const& rhs) \
{                                                                   \
    if(lhs.owner() == rhs.owner())                                  \
    {                                                               \
        if(ctx->pid() == lhs.owner())                               \
            return _##name##_pp(ctx, lhs, rhs)                      \
                .set_visibility(Visibility::Private(lhs.owner()));  \
        else                                                        \
            return lhs;                                             \
    }                                                               \
    else                                                            \
    {                                                               \
        Value lhs_share = _input_p(ctx, lhs);                       \
        Value rhs_share = _input_p(ctx, rhs);                       \
        return ShSh_##name(ctx, lhs_share, rhs_share);              \
    }                                                               \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value ShPr_##name(Context* ctx, Value const& lhs, Value const& rhs) \
{                                                                   \
    Value rhs_share = _input_p(ctx, rhs);                           \
    return ShSh_##name(ctx, lhs, rhs_share);                        \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value ShSh_##name(Context* ctx, Value const& lhs, Value const& rhs) \
{                                                                   \
    return _##name##_ss(ctx, lhs, rhs)                              \
        .set_visibility(Visibility::Share());                       \
}

/// @brief Redirection function of bit shift functions for public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result Value
#define __PRIMITIVE_SHIFT_FUNC_IMPLEMENT__(name)                    \
template <typename Value>                                           \
Value Pb_##name(Context* ctx, Value const& in, std::size_t nbits)   \
{                                                                   \
    return _##name##_p(ctx, in, nbits)                              \
        .set_visibility(Visibility::Public());                      \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value Pr_##name(Context* ctx, Value const& in, std::size_t nbits)   \
{                                                                   \
    if(ctx->pid() == in.owner())                                    \
        return _##name##_p(ctx, in, nbits)                          \
            .set_visibility(Visibility::Private(in.owner()));       \
    else                                                            \
        return in;                                                  \
}                                                                   \
                                                                    \
template <typename Value>                                           \
Value Sh_##name(Context* ctx, Value const& in, std::size_t nbits)   \
{                                                                   \
    return _##name##_s(ctx, in, nbits)                              \
        .set_visibility(Visibility::Share());                       \
}

/// @brief Redirection function of bit decomposition functions public, private and share input respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
#define __PRIMITIVE_BITDEC_FUNC_IMPLEMENT__(name)                                   \
template <typename Value>                                                           \
std::vector<Value> Pb_##name(Context* ctx, Value const& in, std::size_t nbits)      \
{                                                                                   \
    auto ans = _##name##_p(ctx, in, nbits);                                         \
    for(auto& x: ans)   x.set_visibility(Visibility::Public());                     \
    return ans;                                                                     \
}                                                                                   \
                                                                                    \
template <typename Value>                                                           \
std::vector<Value> Pr_##name(Context* ctx, Value const& in, std::size_t nbits)      \
{                                                                                   \
    if(ctx->pid() == in.owner())                                                    \
    {                                                                               \
        auto ans = _##name##_p(ctx, in, nbits);                                     \
        for(auto& x: ans)   x.set_visibility(Visibility::Private(in.owner()));      \
        return ans;                                                                 \
    }                                                                               \
    else                                                                            \
        return std::vector<Value>(nbits, in);                                       \
}                                                                                   \
                                                                                    \
template <typename Value>                                                           \
std::vector<Value> Sh_##name(Context* ctx, Value const& in, std::size_t nbits)      \
{                                                                                   \
    auto ans = _##name##_s(ctx, in, nbits);                                         \
    for(auto& x: ans)   x.set_visibility(Visibility::Share());                      \
    return ans;                                                                     \
}

__PRIMITIVE_UNARY_FUNC_IMPLEMENT__(neg)
__PRIMITIVE_UNARY_FUNC_IMPLEMENT__(msb)
__PRIMITIVE_UNARY_FUNC_IMPLEMENT__(eqz)
__PRIMITIVE_UNARY_FUNC_IMPLEMENT__(square)

__PRIMITIVE_BINARY_FUNC_IMPLEMENT__(add)
__PRIMITIVE_BINARY_FUNC_IMPLEMENT__(mul)

__PRIMITIVE_SHIFT_FUNC_IMPLEMENT__(lshift)
__PRIMITIVE_SHIFT_FUNC_IMPLEMENT__(trunc)

__PRIMITIVE_BITDEC_FUNC_IMPLEMENT__(bitdec)
__PRIMITIVE_BITDEC_FUNC_IMPLEMENT__(h1bitdec)

/// @brief Redirection function of matrix multiplication functions for (public, public), (share, public), (public, share), (share, share), 
///        (private, public), (public, private), (private, private), (share, private) and (private, share) input pair respectively.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
template <typename Value>
Value PbPb_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    return _matmul_pp(ctx, lhs, rhs)
        .set_visibility(Visibility::Public());
}

template <typename Value>
Value ShPb_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    return _matmul_sp(ctx, lhs, rhs)
        .set_visibility(Visibility::Share());
}

template <typename Value>
Value PbSh_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    return _matmul_ps(ctx, lhs, rhs)
        .set_visibility(Visibility::Share());
}

template <typename Value>
Value ShSh_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    return _matmul_ss(ctx, lhs, rhs)
        .set_visibility(Visibility::Share());
}

template <typename Value>
Value PrPb_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value      private_data  = lhs;
    playerid_t private_owner = private_data.owner();

    if(ctx->pid() == private_owner)
        return _matmul_pp(ctx, lhs, rhs)
            .set_visibility(Visibility::Private(private_owner));
    else
        return private_data;
}

template <typename Value>
Value PbPr_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value      private_data  = rhs;
    playerid_t private_owner = private_data.owner();

    if(ctx->pid() == private_owner)
        return _matmul_pp(ctx, lhs, rhs)
            .set_visibility(Visibility::Private(private_owner));
    else
        return private_data;
}

template <typename Value>
Value PrPr_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    if(lhs.owner() == rhs.owner())
    {
        if(ctx->pid() == lhs.owner())
            return _matmul_pp(ctx, lhs, rhs)
                .set_visibility(Visibility::Private(lhs.owner()));
        else
            return lhs;
    }
    else
    {
        Value lhs_share = _input_p(ctx, lhs);
        Value rhs_share = _input_p(ctx, rhs);
        return _matmul_ss(ctx, lhs_share, rhs_share)
            .set_visibility(Visibility::Share());
    }
}

template <typename Value>
Value ShPr_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value lhs_share = lhs;
    Value rhs_share = _input_p(ctx, rhs);
    return _matmul_ss(ctx, lhs_share, rhs_share)
        .set_visibility(Visibility::Share());
}

template <typename Value>
Value PrSh_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value lhs_share = _input_p(ctx, lhs);
    Value rhs_share = rhs;
    return _matmul_ss(ctx, lhs_share, rhs_share)
        .set_visibility(Visibility::Share());
}

} // namespace pppu
