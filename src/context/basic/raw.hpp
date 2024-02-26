#pragma once

#include "raw.h"

#include "primitive.hpp"

namespace pppu
{

/// @brief Redirection function of input based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The share Value
template <typename Value>
Value r_input(Context* ctx, Value const& in)
{
    if(in.is_private() == false)
        throw std::invalid_argument("input data must be private");

    return Pr_input(ctx, in);
}

/// @brief Redirection function of open based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The public Value
template <typename Value>
Value r_open(Context* ctx, Value const& in)
{
    if (in.is_public())
        return in;

    if (in.is_private())
    {
        // throw std::invalid_argument("private data cannot be opened");
        return Sh_open(ctx, Pr_input(ctx, in));
    }

    if (in.is_share() == false)
        throw std::runtime_error("context: invalid visibility");
    
    return Sh_open(ctx, in);
}

/// @brief Redirection function of non-array functions based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The result Value
#define __RAW_UNARY_FUNC_IMPLEMENT__(name)                 \
template <typename Value>                                  \
Value r_##name(Context* ctx, Value const& in)              \
{                                                          \
    if     (in.is_public() )    return Pb_##name(ctx, in); \
    else if(in.is_private())    return Pr_##name(ctx, in); \
    else if(in.is_share()  )    return Sh_##name(ctx, in); \
    else    throw std::invalid_argument("bad data type");  \
}

/// @brief Redirection function of binary functions based on visibility.
/// @param ctx The calculation settings
/// @param lhs First input
/// @param rhs Second input
/// @return The result Value
#define __RAW_BINARY_FUNC_IMPLEMENT__(name)                                               \
template <typename Value>                                                                 \
Value r_##name(Context* ctx, Value const& lhs, Value const& rhs)                          \
{                                                                                         \
    if     ( lhs.is_public()  && rhs.is_public()  )    return PbPb_##name(ctx, lhs, rhs); \
    else if( lhs.is_public()  && rhs.is_private() )    return PrPb_##name(ctx, rhs, lhs); \
    else if( lhs.is_public()  && rhs.is_share()   )    return ShPb_##name(ctx, rhs, lhs); \
                                                                                          \
    else if( lhs.is_private() && rhs.is_public()  )    return PrPb_##name(ctx, lhs, rhs); \
    else if( lhs.is_private() && rhs.is_private() )    return PrPr_##name(ctx, lhs, rhs); \
    else if( lhs.is_private() && rhs.is_share()   )    return ShPr_##name(ctx, rhs, lhs); \
                                                                                          \
    else if ( lhs.is_share()  && rhs.is_public()  )    return ShPb_##name(ctx, lhs, rhs); \
    else if ( lhs.is_share()  && rhs.is_private() )    return ShPr_##name(ctx, lhs, rhs); \
    else if ( lhs.is_share()  && rhs.is_share()   )    return ShSh_##name(ctx, lhs, rhs); \
                                                                                          \
    else    throw std::invalid_argument("bad data type");                                 \
}

/// @brief Redirection function of bit shift functions based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Binary bits to shift
/// @return The result Value
#define __RAW_SHIFT_FUNC_IMPLEMENT__(name)                        \
template <typename Value>                                         \
Value r_##name(Context* ctx, Value const& in, std::size_t nbits)  \
{                                                                 \
    if     (in.is_public() )    return Pb_##name(ctx, in, nbits); \
    else if(in.is_private())    return Pr_##name(ctx, in, nbits); \
    else if(in.is_share()  )    return Sh_##name(ctx, in, nbits); \
    else    throw std::invalid_argument("bad data type");         \
}

/// @brief Redirection function of bit decomposition functions based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
#define __RAW_BITDEC_FUNC_IMPLEMENT__(name)                                   \
template <typename Value>                                                     \
std::vector<Value> r_##name(Context* ctx, Value const& in, std::size_t nbits) \
{                                                                             \
    if     (in.is_public() )    return Pb_##name(ctx, in, nbits);             \
    else if(in.is_private())    return Pr_##name(ctx, in, nbits);             \
    else if(in.is_share()  )    return Sh_##name(ctx, in, nbits);             \
    else    throw std::invalid_argument("bad data type");                     \
}

/// @brief Redirection function of matrix multiplication functions based on visibility.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @param nbits Maximum decomposed binary bits
/// @return The result Value
template <typename Value>
Value r_matmul(Context* ctx, Value const& lhs, Value const& rhs)
{
    if     ( lhs.is_public()  && rhs.is_public()  )    return PbPb_matmul(ctx, lhs, rhs);
    else if( lhs.is_public()  && rhs.is_private() )    return PbPr_matmul(ctx, lhs, rhs);
    else if( lhs.is_public()  && rhs.is_share()   )    return PbSh_matmul(ctx, lhs, rhs);

    else if( lhs.is_private() && rhs.is_public()  )    return PrPb_matmul(ctx, lhs, rhs);
    else if( lhs.is_private() && rhs.is_private() )    return PrPr_matmul(ctx, lhs, rhs);
    else if( lhs.is_private() && rhs.is_share()   )    return PrSh_matmul(ctx, lhs, rhs);

    else if ( lhs.is_share()  && rhs.is_public()  )    return ShPb_matmul(ctx, lhs, rhs);
    else if ( lhs.is_share()  && rhs.is_private() )    return ShPr_matmul(ctx, lhs, rhs);
    else if ( lhs.is_share()  && rhs.is_share()   )    return ShSh_matmul(ctx, lhs, rhs);
}

__RAW_UNARY_FUNC_IMPLEMENT__(neg)
__RAW_UNARY_FUNC_IMPLEMENT__(msb)
__RAW_UNARY_FUNC_IMPLEMENT__(eqz)
__RAW_UNARY_FUNC_IMPLEMENT__(square)

__RAW_BINARY_FUNC_IMPLEMENT__(add)
__RAW_BINARY_FUNC_IMPLEMENT__(mul)

__RAW_SHIFT_FUNC_IMPLEMENT__(lshift)
__RAW_SHIFT_FUNC_IMPLEMENT__(trunc)

__RAW_BITDEC_FUNC_IMPLEMENT__(bitdec)
__RAW_BITDEC_FUNC_IMPLEMENT__(h1bitdec)

} // namespace pppu
