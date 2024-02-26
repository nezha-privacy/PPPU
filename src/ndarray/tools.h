#pragma once

#include "array_ref.hpp"
#include "ndarray_ref.hpp"

namespace core
{
/// @brief Flatten the given NDArrayRef into a one-dimensional array.
/// @param in NDArrayRef to be flattened
/// @return The ArrayRef of the flattened one-dimensional array
/// @note Memory allocation: when this array does not have linear strides.
template <typename dtype>
ArrayRef<dtype> flatten(NDArrayRef<dtype> const& in);

/// @brief Unflatten the given one-dimensional array ArrayRef to a NDArrayRef of the specified shape.
/// @param in The ArrayRef of the one-dimensional array to be unflattened
/// @param shape Specifies the shape, passed as std::vector<int64_t>
/// @return The recovered NDArrayRef object with the specified shape
/// @note Memory allocation: never.
template <typename dtype>
NDArrayRef<dtype> unflatten(ArrayRef<dtype> const& in, std::vector<int64_t> shape);

/// @brief Traverse all arrays in order.
/// @param fn Accepts the following two types of functions
/// @example 1. void fn ( int64_t linear_index, dtype x ), 
///             the first parameter is the linear index and the second parameter is the corresponding element value.
///          2. void fn ( dtype x ).
template <typename dtype, typename Fn>
void for_each(ArrayRef<dtype> const& in, Fn&& fn);

/// @brief Traverse all arrays in order
/// @param fn Accepts the following three types of functions
/// @example 1. void fn ( std::span<const int64_t> nd_index, dtype x ), 
///             the first parameter is the n-dimensional index, and the second parameter is the corresponding element value.
///          2. void fn ( int64_t linear_index, dtype x ), 
///             the first parameter is the linear index and the second parameter is the corresponding element value.
///          3. void fn ( dtype x ).
template <typename dtype, typename Fn>
void for_each(NDArrayRef<dtype> const& in, Fn&& fn);

/// @brief Traverse all the elements in the group in order.
/// @param fn Accepts the following three types of functions
/// @example 1. void fn ( std::span<const int64_t> nd_index, NDArrayRef<dtype> x ), 
///             The first argument is an N-1 dimensional index and
///             the second argument is a one-dimensional array of length in.shape[axis].
///          2. void fn ( int64_t linear_index, NDArrayRef<dtype> x ), 
///             The first argument is a linear index and
///             the second argument is a one-dimensional array of length in.shape[axis].
///          3. void fn ( NDArrayRef<dtype> x ).
template <typename dtype, typename Fn>
void for_each_axis(NDArrayRef<dtype> const& in, int64_t axis, Fn&& fn);

/// @brief A dimensionality reduction operation is performed on the 'in' using 'fn'.
/// @param fn (dtype, dtype) -> dtype
/// @param in The NDArrayRef object of the multidimensional array to be reduced
/// @param axis If none, return one single element
/// @param initial_value Used to add the result of dimension reduction.
/// @param keep_dims If true, the output array will have the same dimension as the input
/// @return Reduced value
template <typename Fn, typename dtype>
requires (std::same_as<std::invoke_result_t<Fn, dtype, dtype>, dtype>)
NDArrayRef<dtype> reduce(
    Fn&& fn,
    NDArrayRef<dtype> const& in,
    std::optional<int64_t> axis = std::optional<int64_t>{},
    dtype initial_value = dtype{},
    bool keep_dims = false);

/// @brief Element wise operation.
/// @param fn(dtype) -> dtype
/// @param in Input value to be applied
/// @return Applied value
template <typename Fn, typename dtype, typename rtype = std::invoke_result_t<Fn, dtype>>
ArrayRef<rtype> apply(Fn&& fn, ArrayRef<dtype> const& in);

/// @brief Element wise operation.
/// @param fn(dtype1, dtype2) -> rtype
/// @param lhs First input value to be applied
/// @param lhs Second input value to be applied
/// @return Applied value
template <typename Fn, typename dtype1, typename dtype2, typename rtype = std::invoke_result_t<Fn, dtype1, dtype2>>
ArrayRef<rtype> apply(Fn&& fn, ArrayRef<dtype1> const& lhs, ArrayRef<dtype2> const& rhs);

/// @brief Element wise operation.
/// @param fn(dtype) -> dtype
/// @param in Input value to be applied
/// @return Applied value
template <typename Fn, typename dtype, typename rtype = std::invoke_result_t<Fn, dtype>>
NDArrayRef<rtype> apply(Fn&& fn, NDArrayRef<dtype> const& in);

/// @brief Element wise operation.
/// @param fn(dtype1, dtype2) -> rtype
/// @param lhs First input value to be applied
/// @param lhs Second input value to be applied
/// @return Applied value
template <typename Fn, typename dtype1, typename dtype2, typename rtype = std::invoke_result_t<Fn, dtype1, dtype2>>
NDArrayRef<rtype> apply(Fn&& fn, NDArrayRef<dtype1> const& lhs, NDArrayRef<dtype2> const& rhs);

} // namespace core
