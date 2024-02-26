#pragma once

#include <functional>

/************************ template expression ************************/

/// @struct UnaryExpression
/// @brief Unary operation expression
/// @tparam Operation unary operation
/// @tparam Operand unary operand
template <typename Operation, typename Operand>
struct UnaryExpression {
    Operand operand;
};

/// @struct BinaryExpression
/// @brief Binary operation expression
/// @tparam Operation binary operation
/// @tparam Operand binary operand
template <typename Operation, typename LhsOperand, typename RhsOperand>
struct BinaryExpression {
    LhsOperand lhs;
    RhsOperand rhs;
};

/************************ traits ************************/

enum class EnumExprType { non_expr,
                          scalar,
                          array,
                          unary_expr,
                          binary_expr };

template <typename T>
struct ExprTraits {
    static constexpr auto expr_type = EnumExprType::non_expr;
};

template <typename Op, typename Operand>
struct ExprTraits<UnaryExpression<Op, Operand>> {
    static constexpr auto expr_type = EnumExprType::unary_expr;
    using ref_type = UnaryExpression<Op, Operand>;
};

template <typename Op, typename Lhs, typename Rhs>
struct ExprTraits<BinaryExpression<Op, Lhs, Rhs>> {
    static constexpr auto expr_type = EnumExprType::binary_expr;
    using ref_type = BinaryExpression<Op, Lhs, Rhs>;
};

/************************ concepts ************************/

template <typename T>
concept ConceptExpr = 
    (std::same_as<T, EnumExprType> == false) && // compile error if remove this line
    (ExprTraits<T>::expr_type != EnumExprType::non_expr);

template <typename T>
concept ConceptExprScalar =
    ConceptExpr<T> &&
    ExprTraits<T>::expr_type == EnumExprType::scalar;

template <typename T>
concept ConceptExprArray =
    ConceptExpr<T> &&
    ExprTraits<T>::expr_type == EnumExprType::array;

template <typename T>
concept ConceptExprCompound =
    ConceptExpr<T> &&
    (ExprTraits<T>::expr_type == EnumExprType::unary_expr ||
     ExprTraits<T>::expr_type == EnumExprType::binary_expr);

/************************ macros for operator definition ************************/

#define __define_unary_operator_expression_template__(op, optypename) \
template <ConceptExpr ExprType>                                       \
requires (!ConceptExprScalar<ExprType>)                               \
auto operator op (ExprType const& expr)                               \
{                                                                     \
    using OperationType = optypename;                                 \
    using ExprRefType = ExprTraits<ExprType>::ref_type;               \
    return UnaryExpression<OperationType, ExprType> { expr };         \
}                                                                     \

#define __define_binary_operator_expression_template__(op, optypename)  \
template <ConceptExpr LhsExprType, ConceptExpr RhsExprType>             \
requires (!ConceptExprScalar<LhsExprType> ||                            \
          !ConceptExprScalar<RhsExprType>)                              \
auto operator op (LhsExprType const& lhs, RhsExprType const& rhs)       \
{                                                                       \
    using OperationType = optypename;                                   \
    using LhsExprRefType = ExprTraits<LhsExprType>::ref_type;           \
    using RhsExprRefType = ExprTraits<RhsExprType>::ref_type;           \
    return BinaryExpression<                                            \
        OperationType, LhsExprRefType, RhsExprRefType                   \
    > {lhs, rhs};                                                       \
}                                                                       \

/************************ only enable needed ones ***********************/

//  __define_unary_operator_expression_template__(~, std::bit_not<>);
// __define_binary_operator_expression_template__(|, std::bit_or<> );
// __define_binary_operator_expression_template__(&, std::bit_and<>);
// __define_binary_operator_expression_template__(^, std::bit_xor<>);

//  __define_unary_operator_expression_template__(-, std::negate<>    );
// __define_binary_operator_expression_template__(+, std::plus<>      );
// __define_binary_operator_expression_template__(-, std::minus<>     );
// __define_binary_operator_expression_template__(*, std::multiplies<>);
// __define_binary_operator_expression_template__(/, std::divides<>   );
// __define_binary_operator_expression_template__(%, std::modulus<>   );

// __define_binary_operator_expression_template__(==, std::equal_to<>     );
// __define_binary_operator_expression_template__(!=, std::not_equal_to<> );
// __define_binary_operator_expression_template__(>=, std::greater_equal<>);
// __define_binary_operator_expression_template__(<=, std::less_equal<>   );
// __define_binary_operator_expression_template__(> , std::greater<>      );
// __define_binary_operator_expression_template__(< , std::less<>         );
