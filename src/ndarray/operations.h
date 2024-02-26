#pragma once

#include <functional>

#include "array_ref.hpp"
#include "ndarray_ref.hpp"

namespace core
{

/// @brief Inverts the given ArrayRef.
/// @param in A constant reference to the input array
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> neg(ArrayRef<dtype> const& in);

/// @brief The given left ArrayRef add the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> add(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs);

/// @brief The given left ArrayRef sub the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> sub(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs);

/// @brief The given left ArrayRef mul the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> mul(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs);

/// @brief Bitwise_not the given ArrayRef.
/// @param in A constant reference to the input array
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> bitwise_not(ArrayRef<dtype> const& in);

/// @brief The given left ArrayRef bitwise_xor the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> bitwise_xor(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs);

/// @brief The given left ArrayRef bitwise_and the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> bitwise_and(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs);

/// @brief Performs matrix multiplication on the given ArrayRef.
/// @param lhs Constant reference for the left operand matrix
/// @param rhs Constant reference for the right operand matrix
/// @param M Number of rows in the result matrix
/// @param K Number of columns in the result matrix
/// @param N Number of columns in the left operand matrix (number of rows in the right operand matrix)
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> matmul(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs, int64_t M, int64_t N, int64_t K);

} // namspace core
