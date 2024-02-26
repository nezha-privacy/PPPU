#pragma once

#include <cassert>

#include "bit_vector.h"

/************************ enable expression template ************************/

#ifdef __bitvector_enable_expression_templates__

template <>
struct ExprTraits<bool>
{
    static constexpr auto expr_type = EnumExprType::scalar;
    using ref_type = bool; // save value for scalar
};

template <>
struct ExprTraits<BitVector>
{
    static constexpr auto expr_type = EnumExprType::array;
    using ref_type = BitVector const &; // save reference for array
};

__define_unary_operator_expression_template__ (~, std::bit_not<>);
__define_binary_operator_expression_template__(&, std::bit_and<>);
__define_binary_operator_expression_template__(|, std::bit_or<>);
__define_binary_operator_expression_template__(^, std::bit_xor<>);

namespace detail
{

using Scalar = bool;
using Array  = BitVector;

inline mp_limb_t limb(Scalar scalar, std::size_t)
{
    static constexpr mp_limb_t zero = 0;
    static constexpr mp_limb_t one = ~zero;
    return scalar ? one : zero;
}

inline mp_limb_t limb(BitVector const &array, std::size_t pos)
{
    return reinterpret_cast<const mp_limb_t *>(array.data())[pos];
}

template <typename Operation, typename Operand>
mp_limb_t limb(UnaryExpression<Operation, Operand> expr, std::size_t pos)
{
    static constexpr Operation op;
    auto x = limb(expr.operand, pos);
    return op(x);
}

template <typename Operation, typename LhsOperand, typename RhsOperand>
mp_limb_t limb(BinaryExpression<Operation, LhsOperand, RhsOperand> expr, std::size_t pos)
{
    static constexpr Operation op;
    auto lhs = limb(expr.lhs, pos);
    auto rhs = limb(expr.rhs, pos);
    return op(lhs, rhs);
}

inline std::size_t size_in_bits(Scalar)
{
    return 0;
}

inline std::size_t size_in_bits(Array const &array)
{
    return array.size();
}

template <typename Operation, typename Operand>
std::size_t size_in_bits(UnaryExpression<Operation, Operand> expr)
{
    return size_in_bits(expr.operand);
}

template <typename Operation, typename LhsOperand, typename RhsOperand>
std::size_t size_in_bits(BinaryExpression<Operation, LhsOperand, RhsOperand> expr)
{
    auto sizel = size_in_bits(expr.lhs);
    auto sizer = size_in_bits(expr.rhs);
    assert(sizel == 0 || sizer == 0 || sizel == sizer);
    return std::max(sizel, sizer);
}

} // namespace detail

/// @brief Expression templates eliminate temporary variables during expression evaluation
/// @tparam ExprType Expression type
/// @param expr Expression
template <ConceptExprCompound ExprType>
BitVector::BitVector(ExprType expr)
{
    *this = expr;
}

/// @brief Expression templates eliminate temporary variables during expression evaluation
/// @tparam ExprType Expression type
/// @param expr Expression
template <ConceptExprCompound ExprType>
BitVector& BitVector::operator=(ExprType expr)
{
    size_type size = detail::size_in_bits(expr);
    if(size == 0) throw std::runtime_error("size zero");

    this->resize(size);
    for(size_type i = 0; i < this->size_in_limbs(); ++i) {
        _vec[i] = detail::limb(expr, i);
    }
    return *this;
}

#endif // #ifdef __bitvector_enable_expression_templates__
