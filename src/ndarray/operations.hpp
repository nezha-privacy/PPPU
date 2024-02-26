#pragma once

#include "operations.h"

#include "tools.hpp"

#include <eigen3/Eigen/Dense>

namespace core
{

/// @brief Inverts the given ArrayRef.
/// @param in A constant reference to the input array
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> neg(ArrayRef<dtype> const& in)
{
    return apply(std::negate<>{}, in);
}

/// @brief The given left ArrayRef add the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> add(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs)
{
    return apply(std::plus<>{}, lhs, rhs);
}

/// @brief The given left ArrayRef sub the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> sub(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs)
{
    return apply(std::minus<>{}, lhs, rhs);
}

/// @brief The given left ArrayRef mul the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> mul(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs)
{
    return apply(std::multiplies<>{}, lhs, rhs);
}

/// @brief Bitwise_not the given ArrayRef.
/// @param in A constant reference to the input array
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> bitwise_not(ArrayRef<dtype> const& in)
{
    return apply(std::bit_not<>{}, in);
}

/// @brief The given left ArrayRef bitwise_xor the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> bitwise_xor(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs)
{
    return apply(std::bit_xor<>{}, lhs, rhs);
}

/// @brief The given left ArrayRef bitwise_and the given right ArrayRef.
/// @param lhs Constant reference for the left operand ArrayRef
/// @param rhs Constant reference for the right operand ArrayRef
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> bitwise_and(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs)
{
    return apply(std::bit_and<>{}, lhs, rhs);
}

/// @brief Performs matrix multiplication on the given ArrayRef.
/// @param lhs Constant reference for the left operand matrix
/// @param rhs Constant reference for the right operand matrix
/// @param M Number of rows in the result matrix
/// @param K Number of columns in the result matrix
/// @param N Number of columns in the left operand matrix (number of rows in the right operand matrix)
/// @return The resulting ArrayRef
template <typename dtype>
ArrayRef<dtype> matmul(ArrayRef<dtype> const& lhs, ArrayRef<dtype> const& rhs, int64_t M, int64_t N, int64_t K)
{
    assert( lhs.numel() == M*N );
    assert( rhs.numel() == N*K );

    if( lhs.stride() == 0 || rhs.stride() == 0 )
        throw std::invalid_argument("stride must be non zero");

    using namespace Eigen;
    // MatrixType is a matrix type that is an Eigen matrix type instantiated using the specified data type dtype, 
    // dynamically sized rows and columns, and row main-order storage
    using MatrixType = Matrix<dtype, Dynamic, Dynamic, RowMajor>;
    using StrideType = Stride<Dynamic, Dynamic>;

    auto new_buffer = std::make_shared<typename ArrayRef<dtype>::BufferType>(M*K);
    auto new_data = new_buffer->data();
    int64_t new_numel = M*K;
    int64_t new_offset = 0;
    int64_t new_stride = 1;

    // Map: Map is a class in the Eigen library that maps external data to Eigen's matrix objects without having to copy the data.
    // const MatrixType: This is the type of the matrix object to be mapped, where MatrixType is the Eigen matrix type generated from the template parameter dtype.
    // 0: indicates that the mapped matrix object is constant, that is, the mapped data cannot be modified.
    // StrideType: Specifies the step type of the matrix to specify the span between elements.
    // mlhs and mrhs: are two matrix objects created with the Map class, corresponding to the left operand lhs and the right operand rhs, respectively.
    Map<const MatrixType, 0, StrideType> mlhs { lhs.data() + lhs.offset(), M, N, StrideType{ lhs.stride()*N, lhs.stride() } };
    Map<const MatrixType, 0, StrideType> mrhs { rhs.data() + rhs.offset(), N, K, StrideType{ rhs.stride()*K, rhs.stride() } };

    Map<MatrixType> mans { new_data, M, K };

    mans = mlhs * mrhs;

    return { std::move(new_buffer), new_numel, new_stride, new_offset };
}

} // namspace core
